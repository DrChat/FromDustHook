using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace bfgui {
	public partial class ProgWnd : Form {
		public ProgWnd() {
			InitializeComponent();
		}

		public void SetProgress(int prog) {
			m_progBar.Value = prog;
		}

		public void SetLabel(string label) {
			m_txtLbl.Text = label;
		}
	}
}
