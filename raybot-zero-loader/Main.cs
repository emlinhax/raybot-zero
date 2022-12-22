using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace raybot_zero_loader
{
    public partial class Main : Form
    {
        public Main()
        {
            InitializeComponent();
        }

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case 0x84:
                    base.WndProc(ref m);
                    if ((int)m.Result == 0x1)
                        m.Result = (IntPtr)0x2;
                    return;
            }

            base.WndProc(ref m);
        }

        private void closeLabel_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void loadBtn_Click(object sender, EventArgs e)
        {
            loadBtn.Text = "please wait...";
            Application.DoEvents();
            if (ServiceInstaller.GetServiceStatus("rbzero") == ServiceState.NotFound)
            {
                ServiceInstaller.InstallAndStart("rbzero", "raybot-zero", Directory.GetCurrentDirectory() + "\\driver\\rbzero.sys");
                System.Threading.Thread.Sleep(500);
                loadBtn.Text = "done!";
                Application.DoEvents();
                System.Threading.Thread.Sleep(2000);
                Environment.Exit(1);
            }
            else
            {
                ServiceInstaller.StopService("rbzero");
                ServiceInstaller.Uninstall("rbzero");
                System.Threading.Thread.Sleep(500);
                loadBtn.Text = "load";
            }
        }

        private void Main_Load(object sender, EventArgs e)
        {
            this.Text = RandomString(16);

            if (ServiceInstaller.GetServiceStatus("rbzero") != ServiceState.NotFound)
            {
                loadBtn.Text = "unload";
                return;
            }

            try
            {
                Directory.CreateDirectory("driver");
                WebClient wc = new WebClient();
                wc.DownloadFile("https://example.com/raybot/rbzero.sys", "driver\\rbzero.sys");
            }
            catch
            {
                MessageBox.Show("could not download the driver!", "error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Environment.Exit(666);
            }
        }

        private static Random random = new Random();

        public static string RandomString(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            return new string(Enumerable.Repeat(chars, length)
                .Select(s => s[random.Next(s.Length)]).ToArray());
        }
    }
}
