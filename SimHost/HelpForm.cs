using System;
using System.Windows.Forms;

using dotNSASM;

namespace SimHost
{
    public partial class HelpForm : Form
    {
        public HelpForm()
        {
            InitializeComponent();
        }

        private void HelpForm_Load(object sender, EventArgs e)
        {
            Text += (" ver" + NSASM.Version);
        }
    }
}
