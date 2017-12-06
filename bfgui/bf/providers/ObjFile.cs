using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace bfgui.bf.providers
{
	/**
	 * OBJ File
	 * Appears to contain data about meshes
	 * Beginning is an index array(?)
	 * Ending is definitely floating point vertex data
	 */
	[FileProvider("obj", "OBJ_")]
	class ObjFile : IFileProvider {

		public string ConvertFile(string oldFile, string oldFileName, string newPath) {
			throw new NotImplementedException();
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
