#pragma once

// the maximum length of a filename
#define FILENAME_LEN 256

class IFileManager
{
public:
	virtual int Open(const char* filename, const char* mode) = 0;
	virtual int Close(bool bReset = true) = 0;
	virtual int Seek(size_t offset, int whence = SEEK_SET) = 0;
	virtual int Tell() = 0;
	virtual int EoF() = 0;
	virtual char* const Name() = 0;
	virtual time_t Date(const char* filename) = 0;
	virtual size_t Read(void* buf, size_t elsize, size_t nelem) = 0;
	virtual int Write(const void* buf, int elsize, int nelem) = 0;
	virtual char* GetS(char* buf, size_t n) = 0;

	virtual int ReadInt32() = 0;
	virtual uint ReadUInt32() = 0;
	virtual short ReadInt16() = 0;
	virtual ushort ReadUInt16() = 0;
	virtual ubyte ReadUByte() = 0;
	virtual sbyte ReadSByte() = 0;
	virtual char ReadChar() = 0;
	virtual short ReadFixAng() = 0;
	virtual float ReadFloat() = 0;
	virtual double ReadDouble() = 0;
	virtual void* ReadBytes(void* buffer, int bufLen) = 0;
	virtual void ReadString(char* buffer, int bufLen) = 0;

	virtual int ReadVector(tFixVector& v) = 0;
	virtual int ReadVector(tDoubleVector& v) = 0;
	virtual int ReadVector(tAngleVector& v) = 0;
	virtual int ReadMatrix(CFixMatrix& v) = 0;
	virtual int ReadMatrix(CDoubleMatrix& m) = 0;

	inline int Read(CFixVector& v) { return ReadVector(v.v); }
	inline int Read(CDoubleVector& v) { return ReadVector(v.v); }
	inline int Read(CAngleVector& v) { return ReadVector(v.v); }
	inline int Read(CFixMatrix& m) { return ReadMatrix(m); }
	inline int Read(CDoubleMatrix& m) { return ReadMatrix(m); }

	virtual int WriteInt32(int v) = 0;
	virtual int WriteUInt32(uint v) = 0;
	virtual int WriteInt16(short v) = 0;
	virtual int WriteUInt16(ushort v) = 0;
	virtual int WriteByte(ubyte v) = 0;
	virtual int WriteSByte(sbyte v) = 0;
	virtual int WriteChar(char v) = 0;
	virtual int WriteFloat(float v) = 0;
	virtual int WriteDouble(double v) = 0;
	virtual void* WriteBytes(void* buffer, int length) = 0;

	inline int Write(int& v) { return WriteInt32(v); }
	inline int Write(uint& v) { return WriteUInt32(v); }
	inline int Write(short& v) { return WriteInt16(v); }
	inline int Write(ushort& v) { return WriteUInt16(v); }
	inline int Write(ubyte& v) { return WriteByte(v); }
	inline int Write(sbyte& v) { return WriteSByte(v); }
	inline int Write(char& v) { return WriteChar(v); }
	inline int Write(float& v) { return WriteFloat(v); }
	inline int Write(double& v) { return WriteDouble(v); }

	virtual void WriteVector(const tAngleVector& v) = 0;
	virtual void WriteVector(const tFixVector& v) = 0;
	virtual void WriteVector(const tDoubleVector& v) = 0;
	inline void Write(const CAngleVector& v) { WriteVector(v.v); }
	inline void Write(const CFixVector& v) { WriteVector(v.v); }
	inline void Write(const CDoubleVector& v) { WriteVector(v.v); }

	virtual void WriteMatrix(const CFixMatrix& m) = 0;
	virtual void WriteMatrix(const CDoubleMatrix& m) = 0;
	inline void Write(const CFixMatrix& m) { WriteMatrix(m); }
	inline void Write(const CDoubleMatrix& m) { WriteMatrix(m); }

	virtual void SplitPath(const char* szFullPath, char* szFolder, char* szFile, char* szExt) = 0;
};
