#ifndef __blockman_h
#define __blockman_h

//------------------------------------------------------------------------------

class CBlockManager {
	private:
		char m_filename [256];
		CSegment* m_oldSegments;
		CSegment* m_newSegments;
		short	m_xlatSegNum [SEGMENT_LIMIT];
		int m_bExtended;

	public:
		void Cut (void);
		
		void Copy (char *filename = null, bool bDelete = false);
		
		void Paste (void);
		
		int Read (char *filename);
		
		void QuickCopy (void);

		void QuickPaste (void);
		
		void Delete (void);

		inline int& Extended (void) { return m_bExtended; }

	private:
		void SetupTransformation (CDoubleMatrix& m, CDoubleVector& o);
		
		short Read (CFileManager& fp);
		
		void Write (CFileManager& fp);

		bool CheckTunnelMaker (void);

		bool Error (int argsFound, int argsNeeded, char* msg, int nSegment, char* szFunction);
	};

extern CBlockManager blockManager;

#endif //__blockman_h

//------------------------------------------------------------------------------
//eof blockmanager.h