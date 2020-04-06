/*  PCSX2 - PS2 Emulator for PCs
*  Copyright (C) 2002-2014  PCSX2 Dev Team
*
*  PCSX2 is free software: you can redistribute it and/or modify it under the terms
*  of the GNU Lesser General Public License as published by the Free Software Found-
*  ation, either version 3 of the License, or (at your option) any later version.
*
*  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
*  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*  PURPOSE.  See the GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License along with PCSX2.
*  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PrecompiledHeader.h"
#include "AsyncFileReader.h"
#include "CompressedFileReaderUtils.h"
#include "ChdFileReader.h"
#include "Pcsx2Types.h"
#include "chd.h"

bool ChdFileReader::CanHandle(const wxString& fileName) {
	bool supported = false;
	if (wxFileName::FileExists(fileName) && fileName.Lower().EndsWith(L".chd")) {
		chd_file* cf = nullptr;
		const chd_error err = chd_open(fileName.data(), CHD_OPEN_READ, nullptr, &cf);
		if (err == CHDERR_NONE) {
			supported = true;
		}
		chd_close(cf);
	}
	return supported;
}

bool ChdFileReader::Open(const wxString& fileName) {
	Close();
	m_filename = fileName;

	const chd_error err = chd_open(fileName.data(), CHD_OPEN_READ, nullptr, &m_chd);
	bool success = false;
	if (m_chd && err == CHDERR_NONE && ReadFileHeader() && InitializeBuffers()) {
		success = true;
	}

	if (!success) {
		Close();
	}
	return success;
}

bool ChdFileReader::ReadFileHeader() {
	if (!m_chd) {
		return false;
	}

	const chd_header* header = chd_get_header(m_chd);

	if (!header) {
		return false;
	}

	m_totalSize = header->logicalbytes;
	m_hunkSize = header->hunkbytes;
	m_hunkCount = header->totalhunks;

	return true;
}

bool ChdFileReader::InitializeBuffers() {
	m_hunkBuffer.resize(m_hunkSize);

	return true;
}

void ChdFileReader::Close() {
	m_filename.Empty();

	if (m_chd) {
		chd_close(m_chd);
		m_chd = nullptr;
	}
}

u32 ChdFileReader::CalculateHunk(u64 byteIndex) const noexcept {
	const u64 hunknum = (byteIndex + m_dataoffset) / m_hunkSize;
	// In practice, this is a safe cast.
	return static_cast<u32>(hunknum);
}

u32 ChdFileReader::CalculateOffsetIntoHunk(u64 byteIndex) const noexcept {
	const u64 offset = (byteIndex + m_dataoffset) % m_hunkSize;
	// In practice, this is a safe cast.
	return static_cast<u32>(offset);
}

u32 ChdFileReader::ReadHunk(u8* dest, u32 hunknum, u32 hunkoffset, u32 maxBytes) {
	if (hunknum > m_hunkCount) {
		return 0;
	}

	u8* hunkBuffer = m_hunkBuffer.data();
	const chd_error err = chd_read(m_chd, hunknum, hunkBuffer);
	if (err != CHDERR_NONE) {
		return 0;
	}

	// Count of bytes in the hunk after hunkoffset which are available to read.
	const u32 bytesAvailableInHunk = m_hunkSize - hunkoffset;
	// Read the rest of the hunk or the number of bytes requested if that's less.
	const u32 bytesRead = bytesAvailableInHunk > maxBytes ? maxBytes : bytesAvailableInHunk;
	std::memcpy(dest, hunkBuffer + hunkoffset, bytesRead);

	return bytesRead;
}

int ChdFileReader::ReadSync(void* pBuffer, uint sector, uint count) {
	if (!m_chd) {
		return 0;
	}

	// Note that, in practice, count will always be 1.  It seems one sector is read
	// per interrupt, even if multiple are requested by the application.

	u8* dest = static_cast<u8*>(pBuffer);
	const u64 startByte = static_cast<u64>(sector) * static_cast<u64>(m_blocksize);
	int remaining = count * m_blocksize;
	u32 hunknum = CalculateHunk(startByte);
	const u32 hunkOffset = CalculateOffsetIntoHunk(startByte);

	// Read first hunk, offset by hunkOffset bytes.
	int bytes = ReadHunk(dest, hunknum, hunkOffset, remaining);
	if (bytes == 0) {
		return 0;
	}
	remaining -= bytes;

	while (remaining > 0) {
		// Read subsequent hunks without offsetting into them.
		const u32 readBytes = ReadHunk(dest + bytes, ++hunknum, 0, remaining);
		if (readBytes == 0) {
			break;
		}
		bytes += readBytes;
		remaining -= readBytes;
	}

	return bytes;
}

void ChdFileReader::BeginRead(void* pBuffer, uint sector, uint count) {
	// TODO: No async support yet, implement as sync.
	m_bytesRead = ReadSync(pBuffer, sector, count);
}

int ChdFileReader::FinishRead() {
	const int res = m_bytesRead;
	m_bytesRead = -1;
	return res;
}

void ChdFileReader::CancelRead() {
	// TODO: No async read support yet.
}
