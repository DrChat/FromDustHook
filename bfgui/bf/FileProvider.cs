using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace bfgui.bf {
	/**
	 * <summary>Provides various functions for a certain filetype
	 */
	public interface IFileProvider {
		string ConvertFile(string oldFile, string oldFileName, string newPath);

		// Does this provider support being displayed in a window?
		// One instance of a file provider will only be expected to deal with one window at the most.
		// If you return false here, user requests will just extract and convert the file by default.
		// File can still be extracted if the user manually requests it by either clicking and dragging or right clicking and extracting
		bool CanDisplayWindow();
		void InitWindow(Form wnd); // Initialize a window with controls needed
		void UpdateWindow(Form wnd); // Update the window (if needed)
		void DestroyWindow(Form wnd); // Called when the user requested the window to be destroyed
	}

	public static class FileProviderList {
		private static ArrayList mProviders = new ArrayList();
		private static Boolean mInitialized = false;

		public static void Init() {
			// Run through all of the classes in the assembly and look for our FileType attribute
			var providers =
				from t in Assembly.GetExecutingAssembly().GetTypes()
				let attribs = t.GetCustomAttributes(typeof(FileProvider), true)
				where attribs != null && attribs.Length > 0
				select new { Type = t, Attribs = attribs.Cast<FileProvider>() };

			foreach (var elem in providers) {
				mProviders.Add(elem.Type);
			}

			mInitialized = true;
		}

		public static bool IsInitialized() {
			return mInitialized;
		}

		public static IFileProvider GetProvider(string ext) {
			// Loop through our list of providers and find one for this extension
			for (int i = 0; i < mProviders.Count; i++) {
				FileProvider attrib = (FileProvider)((Type)mProviders[i]).GetCustomAttributes(typeof(FileProvider), false)[0];
				if (attrib.ext == ext) {
					return (IFileProvider)Activator.CreateInstance((Type)mProviders[i]);
				}
			}

			return null;
		}
	}

	/**
	 * <summary>File Type metadata</summary>
	 */
	[System.AttributeUsage(System.AttributeTargets.Class)]
	public class FileProvider : System.Attribute {
		public string ext;
		public string magic;

		public FileProvider(string ext, string magic) {
			this.ext = ext;
			this.magic = magic;
		}
	}
}
