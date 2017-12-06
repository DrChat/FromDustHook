using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Input;

namespace bfgui
{
	public partial class MainForm : Form {
		private bf.BFFile mCurFile;
		private bool mbFileOpen = false;

		[StructLayout(LayoutKind.Sequential)]
		public struct MARGINS {
			public int Left;
			public int Right;
			public int Top;
			public int Bottom;
		}

		[DllImport("dwmapi.dll", PreserveSig = false)]
		public static extern void DwmExtendFrameIntoClientArea
						(IntPtr hwnd, ref MARGINS margins);

		[DllImport("dwmapi.dll", PreserveSig = false)]
		public static extern bool DwmIsCompositionEnabled();

		[DllImport("Shlwapi.dll", CharSet = CharSet.Auto)]
		public static extern long StrFormatByteSize(long fileSize, [MarshalAs(UnmanagedType.LPTStr)]StringBuilder buffer, int bufferSize);

		public MainForm()
		{
			InitializeComponent();

			mMenuFileOpen.Click += OnOpen_Click;
			mFileList.Activation = ItemActivation.Standard;
			mFileList.FullRowSelect = true;
			mFileList.ItemActivate += ListView_ItemActivate;
			mFileList.MouseClick += ListView_MouseClick;
			mFileList.ItemDrag += ListView_ItemDrag;
			mFileListCtx_Extract.Click += ListViewCtxMenu_Extract;

			// Initialize the file provider list
			bf.FileProviderList.Init();

			// Extend the aero effect
			if (DwmIsCompositionEnabled()) {
				MARGINS m = new MARGINS();
				m.Top = 20;
				m.Left = 20;
				//DwmExtendFrameIntoClientArea(this.Handle, ref m);
			}
		}

		public void OpenArchive(string name) {
			if (mCurFile != null)
				mCurFile.Close();

			mFileList.Items.Clear();

			ProgWnd progWnd = new ProgWnd();
			progWnd.Location = new Point(this.Location.X + ((this.Width / 2) - progWnd.Width / 2), this.Location.Y + ((this.Height / 2) - progWnd.Height / 2));
			progWnd.SetLabel("Loading...");
			progWnd.Show();

			ArrayList itemArr = new ArrayList();

			BackgroundWorker worker = new BackgroundWorker();
			worker.WorkerReportsProgress = true;
			worker.DoWork += new DoWorkEventHandler((object sender, DoWorkEventArgs e) => {
				worker.ReportProgress(0);
				mCurFile = new bf.BFFile();

				try {
					mCurFile.Open(name);
				} catch (IOException ex) {
					MessageBox.Show("Could not open file!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
					return;
				}

				if (worker.CancellationPending) {
					mCurFile.Close();
					return;
				}

				worker.ReportProgress(20);

				mbFileOpen = true;

				// 20% -> 80%
				for (int i = 0; i < mCurFile.GetFileCount(); i++) {
					var record = mCurFile.GetFileRecord(i);

					ListViewItem item = new ListViewItem();
					item.Text = record.name;
					item.Tag = i; // Tag the item with the position in the archive itself
					item.SubItems.Add(Path.GetExtension(record.name));

					// File size
					StringBuilder sb = new StringBuilder(256);
					StrFormatByteSize(record.fileSize, sb, 256);

					item.SubItems.Add(sb.ToString());

					item.SubItems.Add(String.Format("0x{0:X}", record.fileOffset.ToString("X8")));
					itemArr.Add(item);

					worker.ReportProgress((int)Math.Floor(20.0 + (((double)i/mCurFile.GetFileCount()) * 60.0)));
					if (worker.CancellationPending) {
						mbFileOpen = false;
						mCurFile.Close();
					}
				}
			});

			worker.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender, RunWorkerCompletedEventArgs e) => {
				// 80% -> 100%
				mFileList.BeginUpdate();
				foreach (ListViewItem item in itemArr) {
					mFileList.Items.Add(item);
				}
				mFileList.EndUpdate();

				progWnd.SetProgress(100);

				progWnd.Close();
			});

			worker.ProgressChanged += new ProgressChangedEventHandler((object sender, ProgressChangedEventArgs e) => {
				progWnd.SetProgress(e.ProgressPercentage);
			});

			worker.RunWorkerAsync();
		}

		public void CloseArchive() {
			mbFileOpen = false;
			mCurFile = null;

			mFileList.Items.Clear();
		}

		/************************
		 * EVENTS
		 ************************/

		private void OnOpen_Click(object sender, System.EventArgs e) {
			OpenFileDialog dlg = new OpenFileDialog();
			dlg.DefaultExt = "*.lin.bf";
			dlg.Filter = "lin.bf Archive Files (*.lin.bf)|*.lin.bf";
			dlg.FilterIndex = 1;
			dlg.Multiselect = false;
			dlg.InitialDirectory = Directory.GetCurrentDirectory();

			if (dlg.ShowDialog() == DialogResult.OK) {
				OpenArchive(dlg.FileName);
			}
		}

		// List of files with headers
		private static string[] headerFiles = {
			".cfg",
			".grp",
			".mdl",
			".son", // Wave sound
			".efg",
			//".tdt", // Texture data?
			".link",
			".feu", // Script file(?)
			".oli", // Translation file
		};

		private string ExtractFile(int id) {
			bf.BFFile.bffilerecord rec = mCurFile.GetFileRecord(id);

			var fn = rec.name;
			var ext = Path.GetExtension(fn);

			string path = Path.Combine(new string[] { Path.GetTempPath(), fn });
			if (File.Exists(path)) {
				File.Delete(path);
			}

			int offset = 0;
			if (headerFiles.Contains(ext))
				offset = 32; // 32 byte header at beginning of file

			FileStream s = mCurFile.ExtractFile(id, path, offset);
			s.Close();

			// Now convert the file
			bf.IFileProvider prov = bf.FileProviderList.GetProvider(ext.Substring(1));
			if (prov != null) {
				path = prov.ConvertFile(path, rec.name, Path.GetTempPath());
			}

			// rename son files because they're .wav format
			if (ext == ".son") {
				string newPath = Path.Combine(new string[] { Path.GetDirectoryName(path), Path.GetFileNameWithoutExtension(path) + ".wav" });
				if (File.Exists(newPath)) {
					File.Delete(newPath);
				}

				File.Move(path, newPath);

				path = newPath;
			}

			return path;
		}

		private void ListView_ItemActivate(object sender, System.EventArgs e) {
			try {
				var path = ExtractFile((int)mFileList.SelectedItems[0].Tag);
				if (path == null) {
					MessageBox.Show("Could not extract the file! :(");
					return;
				}

				try {
					Process.Start(path);
				} catch (Win32Exception ex) {
					// Try the shell's open with dialog
					var procInfo = new ProcessStartInfo(path);

					procInfo.Verb = "openas";
					Process.Start(procInfo);
				}
			} catch (IOException ex) {
				MessageBox.Show("Could not extract the file! :(");
			}
		}

		private void ListView_ItemDrag(object sender, ItemDragEventArgs e) {
			string[] paths = new string[mFileList.SelectedIndices.Count];
			for (int i = 0; i < mFileList.SelectedIndices.Count; i++) {
				string f = ExtractFile((int)mFileList.SelectedItems[i].Tag);

				if (f != null)
					paths[i] = f;
			}

			DoDragDrop(new DataObject(DataFormats.FileDrop, paths), DragDropEffects.Copy);
		}

		private void ListView_MouseClick(object sender, MouseEventArgs e) {
			if (e.Button == MouseButtons.Right) {
				if (mFileList.FocusedItem.Bounds.Contains(e.Location)) {
					mFileListCtxMenu.Show(Cursor.Position);
				}
			}
		}

		/*private class ItemComparer : IComparer<T> {

		}*/

		private void ListView_ColumnClick(object sender, ColumnClickEventArgs e) {

		}

		private void ListViewCtxMenu_Extract(object sender, System.EventArgs e) {
			MessageBox.Show("Unimplemented :(");
		}
	}
}
