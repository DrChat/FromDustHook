namespace bfgui
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			this.mFileList = new System.Windows.Forms.ListView();
			this.mFileList_ColName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.mFileList_ColExt = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.mFileList_ColSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.mFormMenu = new System.Windows.Forms.MainMenu(this.components);
			this.mMenuFile = new System.Windows.Forms.MenuItem();
			this.mMenuFileOpen = new System.Windows.Forms.MenuItem();
			this.mMenuFileExit = new System.Windows.Forms.MenuItem();
			this.mFileListCtxMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.mFileListCtx_Extract = new System.Windows.Forms.ToolStripMenuItem();
			this.mFileList_ColOffset = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.mFileListCtxMenu.SuspendLayout();
			this.SuspendLayout();
			// 
			// mFileList
			// 
			this.mFileList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.mFileList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.mFileList_ColName,
            this.mFileList_ColExt,
            this.mFileList_ColSize,
            this.mFileList_ColOffset});
			this.mFileList.Location = new System.Drawing.Point(12, 12);
			this.mFileList.Name = "mFileList";
			this.mFileList.Size = new System.Drawing.Size(670, 600);
			this.mFileList.TabIndex = 0;
			this.mFileList.UseCompatibleStateImageBehavior = false;
			this.mFileList.View = System.Windows.Forms.View.Details;
			// 
			// mFileList_ColName
			// 
			this.mFileList_ColName.Text = "Name";
			this.mFileList_ColName.Width = 341;
			// 
			// mFileList_ColExt
			// 
			this.mFileList_ColExt.Text = "Extension";
			// 
			// mFileList_ColSize
			// 
			this.mFileList_ColSize.Text = "Size";
			this.mFileList_ColSize.Width = 63;
			// 
			// mFormMenu
			// 
			this.mFormMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.mMenuFile});
			// 
			// mMenuFile
			// 
			this.mMenuFile.Index = 0;
			this.mMenuFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.mMenuFileOpen,
            this.mMenuFileExit});
			this.mMenuFile.Text = "&File";
			// 
			// mMenuFileOpen
			// 
			this.mMenuFileOpen.Index = 0;
			this.mMenuFileOpen.Text = "&Open";
			// 
			// mMenuFileExit
			// 
			this.mMenuFileExit.Index = 1;
			this.mMenuFileExit.Text = "E&xit";
			// 
			// mFileListCtxMenu
			// 
			this.mFileListCtxMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mFileListCtx_Extract});
			this.mFileListCtxMenu.Name = "mFileListCtxMenu";
			this.mFileListCtxMenu.Size = new System.Drawing.Size(119, 26);
			// 
			// mFileListCtx_Extract
			// 
			this.mFileListCtx_Extract.Name = "mFileListCtx_Extract";
			this.mFileListCtx_Extract.Size = new System.Drawing.Size(118, 22);
			this.mFileListCtx_Extract.Text = "&Extract...";
			// 
			// mFileList_ColOffset
			// 
			this.mFileList_ColOffset.Text = "Offset";
			this.mFileList_ColOffset.Width = 83;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(694, 649);
			this.Controls.Add(this.mFileList);
			this.MaximizeBox = false;
			this.Menu = this.mFormMenu;
			this.Name = "MainForm";
			this.Text = "BFTool";
			this.mFileListCtxMenu.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView mFileList;
        private System.Windows.Forms.MainMenu mFormMenu;
        private System.Windows.Forms.MenuItem mMenuFile;
        private System.Windows.Forms.MenuItem mMenuFileExit;
        private System.Windows.Forms.MenuItem mMenuFileOpen;
        private System.Windows.Forms.ColumnHeader mFileList_ColName;
        private System.Windows.Forms.ColumnHeader mFileList_ColSize;
		private System.Windows.Forms.ContextMenuStrip mFileListCtxMenu;
		private System.Windows.Forms.ToolStripMenuItem mFileListCtx_Extract;
		private System.Windows.Forms.ColumnHeader mFileList_ColExt;
		private System.Windows.Forms.ColumnHeader mFileList_ColOffset;
    }
}

