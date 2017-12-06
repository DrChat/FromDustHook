namespace bfgui {
	partial class ProgWnd {
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing) {
			if (disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			this.m_progBar = new System.Windows.Forms.ProgressBar();
			this.m_txtLbl = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// m_progBar
			// 
			this.m_progBar.Location = new System.Drawing.Point(12, 44);
			this.m_progBar.Name = "m_progBar";
			this.m_progBar.Size = new System.Drawing.Size(241, 23);
			this.m_progBar.TabIndex = 0;
			// 
			// m_txtLbl
			// 
			this.m_txtLbl.AutoSize = true;
			this.m_txtLbl.Location = new System.Drawing.Point(12, 19);
			this.m_txtLbl.Name = "m_txtLbl";
			this.m_txtLbl.Size = new System.Drawing.Size(73, 13);
			this.m_txtLbl.TabIndex = 1;
			this.m_txtLbl.Text = "Please Wait...";
			// 
			// ProgWnd
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(265, 79);
			this.ControlBox = false;
			this.Controls.Add(this.m_txtLbl);
			this.Controls.Add(this.m_progBar);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ProgWnd";
			this.Text = "ProgWnd";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ProgressBar m_progBar;
		private System.Windows.Forms.Label m_txtLbl;
	}
}