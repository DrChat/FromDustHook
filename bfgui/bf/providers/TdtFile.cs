using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace bfgui.bf.providers {
	/**
	 * TDT File
	 * Appears to hold texture data for the game (bitmap format?)
	 * Version(?) at 0x0 (always 2)
	 * Pixel data appears to start at 0x10
	 * Resolution int16 at 0xA and 0xC?
	 */
	[FileProvider("tdt", "TDT_")]
	class TdtFile : IFileProvider {
		public string ConvertFile(string oldFile, string oldFileName, string newPath) {
			FileStream fs = new FileStream(oldFile, FileMode.Open);
			BinaryReader br = new BinaryReader(fs);

			string newFn = Path.Combine(new string[] { newPath, Path.GetFileNameWithoutExtension(oldFileName) + ".bmp" });
			FileStream nfs = new FileStream(newFn, FileMode.Create);

			fs.Position = 0x2A;
			ushort width = br.ReadUInt16();
			ushort height = br.ReadUInt16();

			// Read binary data
			fs.Position = 0x30;

			long buffSize = fs.Length - fs.Position;
			byte[] arr = new byte[buffSize + 4096];
			fs.Read(arr, 0, (int)buffSize);

			for (long i = buffSize; i < buffSize + 4096; i += 3) {
				arr[i] = 0xFF; // R
				arr[i + 1] = 0x0; // G
				arr[i + 2] = 0x0; // B
			}

			// TODO: So these files appear to be compressed with DXT as according to some filenames
			// So we need to decompress these files... Need to find a library that can do this.

			try {
				unsafe {
					fixed (byte *ptr = arr) {
						Bitmap bm = new Bitmap(width, height, width, PixelFormat.Format24bppRgb, (IntPtr)ptr);
						bm.Save(nfs, ImageFormat.Bmp);
					}
				}
			} catch (ExternalException e) {
				fs.Close();
				nfs.Close();

				return null;
			}

			fs.Close();
			nfs.Close();

			return newFn;
		}

		public bool CanDisplayWindow() {
			return false;
		}

		public void InitWindow(System.Windows.Forms.Form wnd) {
			throw new NotImplementedException();
		}

		public void UpdateWindow(System.Windows.Forms.Form wnd) {
			throw new NotImplementedException();
		}

		public void DestroyWindow(System.Windows.Forms.Form wnd) {
			throw new NotImplementedException();
		}
	}
}
