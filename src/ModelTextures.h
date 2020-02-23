#ifndef _ModelTextures_h
#define _ModelTextures_h

class CModelTextures {
	private:
		int								m_nTextures;
		CDynamicArray<CCharArray>	m_names;
		CDynamicArray<CTexture>		m_textures;
		CDynamicArray<ubyte>			m_nTeam;

	public:
		CModelTextures () { Init (); } 
		void Init (void) { m_nTextures = 0; }
		int Bind (int bCustom);
		void Release (void);
		int Read (char* pszFolder);
		int ReadBinary (CFileManager& fp);
		void WriteBinary (CFileManager& fp);
		bool Create (int nTextures);
		void Destroy (void);
	
		inline int Count (void) { return m_nTextures; }
		inline CTexture& Texture (int i) { return m_textures [i]; }
		inline CCharArray& Name (int i) { return m_names [i]; }
		inline byte& Team (int i) { return m_nTeam [i]; }

		inline CDynamicArray<CTexture>* Textures (void) { return &m_textures; }
	};

#endif