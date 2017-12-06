using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bfgui {
	static class Misc {
		/**
		 * <summary>Copy a stream to another stream by the specified amount of bytes.</summary>
		 */
		public static void CopyStreamToStream(Stream a, Stream b, uint bytesToCopy, uint bufferSize) {
			byte[] buff = new byte[bufferSize];
			long curPos = 0;

			while (curPos < bytesToCopy) {
				// Calc the amount of bytes to copy over
				long numBytes = bufferSize < bytesToCopy - curPos ? bufferSize : bytesToCopy - curPos;
				curPos += numBytes;

				a.Read(buff, 0, (int)numBytes);
				b.Write(buff, 0, (int)numBytes);
			}
		}
	}
}
