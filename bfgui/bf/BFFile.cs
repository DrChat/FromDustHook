using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Collections;

// Disable this annoying warning
#pragma warning disable 0649

namespace bfgui.bf
{
	class BFFile
	{
		// 5184 bytes
		public struct bfheader {
			public UInt32           magic; // 0
			[MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 8)]
			public string           unk1; // 4
			public int              unk2;   // 12 always 1?
			public int		        unk3;	// 16 always 1?
			public int		        unk4;	// 20 always 0?
			public uint		        headerSize;	// 24 appears to be constant; size starting from the magic number. Could call this the first file offset too
			public int		        unk6;	// 28
			public int		        unk7;	// 32
			public int		        unk8;	// 36
			public int		        unk9;	// 40 always 1?
			public uint		        fileCount;	// 44
			public int		        unk11;	// 48 always 1?
			[MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 12)]
			public string   	    unk12;	// 52 always filled 0xFF?
			public int		        unk13;	// 64 always filled 0x7F?
			[MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 4)]
			public string   	    unk14;	// 68 always 0x91022016?
			[MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 512)]
			public string   	    dummyfill;	// 73 filled with 0x7F
			[MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 4600)]
			public string   	    dummyfill2;	// 584 filled with 0x00
		};

		// 200 bytes
		public struct bffilerecord {
			[MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 64)]
			public string	name; // unused data filled with 0x7F, null-terminated string. name[63] is also null-terminated
			public int		unk2; // 64
			public int		unk3; // 68
			public int		unk4; // 72
			public int		unk5; // 76
			public int		unk6; // 80
			public int		id; // 84 file id maybe? starting from -1
			public uint		fileSize; // 88 size of entire file (inc. header at begin of some files)
			public float	unk9; // 92 appears to be a float. Near 195 increases as records go on
			public int		unk10; // 96 same every rec?
			public int		unk11; // 100
			public int		unk12; // 104 always 0?
			public uint		fileOffset; // 108 offset to data from begin of file
			public int		unk13; // 112 always 0?
			public int		unk14; // 116 same as fileSize (always?)
			public int		unk15; // 120 always 0x00007F7F?
			[MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 64)]
			public string	unk16; // 124 0-filled data
			public int 	    unk17; // 188 always 0?
			public int		unk18; // 192 always 128?
			public int		unk19; // 196 always 0x7F?
		};

		// 32 bytes
		public struct bffileheader {
			int		size; // 0 size after this header
			int		unk2; // 4 size again?
			int		unk3; // 8
			int		unk4; // 12
			int		unk5; // 16
			[MarshalAsAttribute(UnmanagedType.ByValTStr, SizeConst = 4)]
			string	magic; // 20
			int		unk6; // 24
			int		unk7; // 28
		};

		// Credits: http://code.cheesydesign.com/?p=572
		public static T FromBinaryReader<T>(BinaryReader reader) {
			// Read in a byte array
			byte[] bytes = reader.ReadBytes(Marshal.SizeOf(typeof(T)));

			// Pin the managed memory while, copy it out the data, then unpin it
			GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
			T theStructure = (T)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), typeof(T));
			handle.Free();

			return theStructure;
		}

		public static T FromStream<T>(Stream s) {
			var br = new BinaryReader(s);
			return FromBinaryReader<T>(br);
		}

		private uint mFileCount;
		private Stream mStream;
		private bfheader mHeader;
		private ArrayList mFileRecords = new ArrayList();

		private bool ReadLinRecords(Stream s) {
			var br = new BinaryReader(s);
			mHeader = FromBinaryReader<bfheader>(br);

			if (mHeader.magic != 4538945) { // == "ABE\0"
				return false;
			}

			mFileCount = mHeader.fileCount;

			for (int i = 0; i < mFileCount; i++) {
				// Set the stream position to the beginning of the file record (not really necessary cause they're right next to eachother but whatever)
				// Offset + 12 cause there's some junk right after the header
				s.Position = mHeader.headerSize + 12 + (i * Marshal.SizeOf(typeof(bffilerecord)));
				var record = FromStream<bffilerecord>(s);
				mFileRecords.Add(record);
			}

			return true;
		}

		public bool Open(string path) {
			mStream = new FileStream(path, FileMode.Open);
			return ReadLinRecords(mStream);
		}

		public bool Open(Stream file) {
			mStream = file;
			return ReadLinRecords(mStream);
		}

		public void Close() {
			mStream.Close();
			mFileCount = 0;

			//mHeader = null;
			mFileRecords.Clear();
		}

		public uint GetFileCount() {
			return mFileCount;
		}

		public bffilerecord GetFileRecord(int i) {
			return (bffilerecord)mFileRecords[i];
		}

		public Stream OpenSubFile(int i) {
			bffilerecord record = GetFileRecord(i);
			mStream.Position = record.fileOffset;

			var stream = new BufferedStream(mStream, (int)record.fileSize);
			stream.SetLength(record.fileSize);
			return stream;
		}

		public FileStream ExtractFile(int i, string targetPath, int offset = 0) {
			bffilerecord record = GetFileRecord(i);
			mStream.Position = record.fileOffset + offset;

			var stream = new FileStream(targetPath, FileMode.CreateNew);

			// Buffered transfer contents over
			byte [] buffer = new byte[1024];
			while (mStream.Position < record.fileOffset + record.fileSize) {
				long fileEnd = record.fileOffset + record.fileSize;
				long numBytes = fileEnd - mStream.Position < 1024 ? fileEnd - mStream.Position : 1024;

				mStream.Read(buffer, 0, (int)numBytes);
				stream.Write(buffer, 0, (int)numBytes);
			}

			return stream;
		}
	}
}
