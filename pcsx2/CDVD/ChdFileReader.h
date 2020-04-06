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

#pragma once

#include "AsyncFileReader.h"

#include <vector>

typedef struct _chd_file chd_file;

class ChdFileReader : public AsyncFileReader
{
	DeclareNoncopyableObject(ChdFileReader);
public:
	ChdFileReader(void) noexcept :
		m_chd(nullptr),
		m_totalSize(0),
		m_hunkCount(0),
		m_hunkSize(0),
		m_bytesRead(0) {
		m_blocksize = 2048;
		m_dataoffset = 0;
	}

	virtual ~ChdFileReader(void) { Close(); };

	static  bool CanHandle(const wxString& fileName);
	bool Open(const wxString& fileName) override;

	int ReadSync(void* pBuffer, uint sector, uint count) override;

	void BeginRead(void* pBuffer, uint sector, uint count) override;
	int FinishRead(void) override;
	void CancelRead(void) override;

	void Close(void) override;

	uint GetBlockCount(void) const noexcept override {
		return (m_totalSize - m_dataoffset) / m_blocksize;
	}

	void SetBlockSize(uint bytes) noexcept override { m_blocksize = bytes; }
	void SetDataOffset(int bytes) noexcept override { m_dataoffset = bytes; }

private:
	bool ReadFileHeader();
	bool InitializeBuffers();

	// Read hunk at hunknum and store up to maxBytes of the hunk in dest.
	// Source bytes from hunk are offset by hunkoffset.
	u32 ReadHunk(u8* dest, u32 hunknum, u32 hunkoffset, u32 maxBytes);

	// Calculate the hunk index which contains byteIndex.
	u32 CalculateHunk(u64 byteIndex) const noexcept;

	// Calculate the offset in the hunk at which byteIndex begins.
	u32 CalculateOffsetIntoHunk(u64 byteIndex) const noexcept;

	// Underlying CHD file.
	chd_file* m_chd;

	// Logical size in bytes of the compressed data.
	u64 m_totalSize;
	// Total number of hunks in the CHD.
	u32 m_hunkCount;
	// Size of each hunk in bytes.
	u32 m_hunkSize;

	// The result of a read is stored here between BeginRead() and FinishRead().
	int m_bytesRead;
	// We read the CHD one hunk at a time into this buffer.
	std::vector<u8> m_hunkBuffer;
};
