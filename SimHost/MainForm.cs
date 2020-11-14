using System;
using System.Threading;
using System.Windows.Forms;
using System.Collections.Generic;

using dotNSASM;
using HID;

namespace SimHost
{
    public partial class MainForm : Form
    {
        class SimCore : NSASM
        {
            readonly List<byte[]> byteCode;

            SimCore(string[][] code) : base(16, 16, 8, code)
            {
                byteCode = new List<byte[]>();
            }

            public static SimCore GetSimCore(string code)
            {
                var c = Util.GetSegments(code);
                return new SimCore(c);
            }

            protected override NSASM Instance(NSASM super, string[][] code)
            {
                return new SimCore(code);
            }

            public byte[][] GetBytes()
            {
                return byteCode.ToArray();
            }

            protected override void LoadFuncList()
            {
                funcList.Add("nop", (dst, src, ext) =>
                {
                    if (dst == null && src == null)
                    {
                        byteCode.Add(new byte[] { 0x00 });
                        return Result.OK;
                    }
                    return Result.ERR;
                });

                funcList.Add("jmp", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst == null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;

                    ushort i = (ushort)(((int)dst.data) & 0xFFFF);
                    byteCode.Add(new byte[] { 
                        0x01,
                        0x00,
                        (byte)(i & 0x00FF),
                        (byte)((i & 0xFF00) >> 8) 
                    });

                    return Result.OK;
                });

                funcList.Add("clr", (dst, src, ext) =>
                {
                    if (dst == null && src == null)
                    {
                        byteCode.Add(new byte[] { 0x02 });
                        return Result.OK;
                    }
                    return Result.ERR;
                });

                funcList.Add("prt", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst == null) return Result.ERR;
                    if (dst.type != RegType.CHAR && dst.type != RegType.INT && dst.type != RegType.STR)
                        return Result.ERR;

                    if (dst.type == RegType.STR)
                    {
                        string str = (string)dst.data;
                        for (int i = 0; i < str.Length; i++)
                        {
                            byteCode.Add(new byte[] {
                                0x03,
                                (byte)str[i]
                            });
                        }
                    }
                    else
                    {
                        byte i;
                        if (dst.type == RegType.CHAR)
                            i = (byte)(((char)dst.data) & 0xFF);
                        else
                            i = (byte)(((int)dst.data) & 0xFF);
                        byteCode.Add(new byte[] {
                            0x03,
                            i
                        });
                    }

                    return Result.OK;
                });

                funcList.Add("hidp", (dst, src, ext) =>
                {
                    if (dst == null && src == null)
                    {
                        byteCode.Add(new byte[] { 0x04 });
                        return Result.OK;
                    }
                    return Result.ERR;
                });

                funcList.Add("strp", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst == null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    byteCode.Add(new byte[] {
                        0x05,
                        i
                    });

                    return Result.OK;
                });

                funcList.Add("out", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    byte j = (byte)(((int)src.data) & 0xFF);
                    byteCode.Add(new byte[] {
                        0x06,
                        i,
                        j,
                        0x00
                    });

                    return Result.OK;
                });

                funcList.Add("keyp", (dst, src, ext) =>
                {
                    if (dst == null && src == null)
                    {
                        byteCode.Add(new byte[] { 0x07 });
                        return Result.OK;
                    }
                    return Result.ERR;
                });

                funcList.Add("ldi", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    ushort j = (ushort)(((int)src.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x08,
                        i,
                        (byte)(j & 0x00FF),
                        (byte)((j & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });

                funcList.Add("lde", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    ushort j = (ushort)(((int)src.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x09,
                        i,
                        (byte)(j & 0x00FF),
                        (byte)((j & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });

                funcList.Add("wri", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)src.data) & 0xFF);
                    ushort j = (ushort)(((int)dst.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x0A,
                        i,
                        (byte)(j & 0x00FF),
                        (byte)((j & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });

                funcList.Add("wre", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)src.data) & 0xFF);
                    ushort j = (ushort)(((int)dst.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x0B,
                        i,
                        (byte)(j & 0x00FF),
                        (byte)((j & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });

                funcList.Add("sleep", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst == null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;

                    ushort i = (ushort)(((int)dst.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x0C,
                        0x00,
                        (byte)(i & 0x00FF),
                        (byte)((i & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });

                funcList.Add("led", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    int j = ((int)src.data) & 0xFFFFFF;

                    byte r = (byte)((j & 0xFF0000) >> 16);
                    byte g = (byte)((j & 0x00FF00) >> 8);
                    byte b = (byte)((j & 0x0000FF) >> 0);

                    byte e = (byte)((r & 0x07) << 5 | (g & 0x03) << 3 | (b & 0x07));
                    ushort c = (ushort)((r & 0xF8) << 8 | (g & 0xFC) << 3 | (b & 0xF8) >> 3);

                    byteCode.Add(new byte[] {
                        0x0D,
                        0x00,
                        i,
                        e,
                        0x00,
                        (byte)(c & 0x00FF),
                        (byte)((c & 0xFF00) >> 8),
                        0x00
                    });

                    return Result.OK;
                });

                funcList.Add("time", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst == null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;

                    ushort i = (ushort)(((int)dst.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x0E,
                        0x00,
                        (byte)(i & 0x00FF),
                        (byte)((i & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });

                funcList.Add("fade", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    int j = ((int)src.data) & 0xFFFFFF;

                    byte r = (byte)((j & 0xFF0000) >> 16);
                    byte g = (byte)((j & 0x00FF00) >> 8);
                    byte b = (byte)((j & 0x0000FF) >> 0);

                    byte e = (byte)((r & 0x07) << 5 | (g & 0x03) << 3 | (b & 0x07));
                    ushort c = (ushort)((r & 0xF8) << 8 | (g & 0xFC) << 3 | (b & 0xF8) >> 3);

                    byteCode.Add(new byte[] {
                        0x0F,
                        0x00,
                        i,
                        e,
                        0x00,
                        (byte)(c & 0x00FF),
                        (byte)((c & 0xFF00) >> 8),
                        0x00
                    });

                    return Result.OK;
                });

                funcList.Add("trig", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    byte j = (byte)(((int)src.data) & 0xFF);
                    byteCode.Add(new byte[] {
                        0x10,
                        i,
                        j,
                        0x00
                    });

                    return Result.OK;
                });

                funcList.Add("rgb", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    byte j = (byte)(((int)src.data) & 0xFF);
                    byteCode.Add(new byte[] {
                        0x11,
                        i,
                        j,
                        0x00
                    });

                    return Result.OK;
                });

                funcList.Add("sysrst", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    ushort j = (ushort)(((int)src.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x12,
                        i,
                        (byte)(j & 0x00FF),
                        (byte)((j & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });

                funcList.Add("reload", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    ushort j = (ushort)(((int)src.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x13,
                        i,
                        (byte)(j & 0x00FF),
                        (byte)((j & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });

                funcList.Add("iap", (dst, src, ext) =>
                {
                    if (src != null) return Result.ERR;
                    if (dst != null) return Result.ERR;
                    if (dst.type != RegType.INT) return Result.ERR;
                    if (src.type != RegType.INT) return Result.ERR;

                    byte i = (byte)(((int)dst.data) & 0xFF);
                    ushort j = (ushort)(((int)src.data) & 0xFFFF);
                    byteCode.Add(new byte[] {
                        0x14,
                        i,
                        (byte)(j & 0x00FF),
                        (byte)((j & 0xFF00) >> 8)
                    });

                    return Result.OK;
                });
            }

            protected override void LoadParamList()
            {
            
            }
        }

        Hid hid;
        IntPtr ptr;

        public MainForm()
        {
            InitializeComponent();
            devList.SelectedIndex = 0;

            hid = new Hid();
            hid.DataReceived += Hid_DataReceived;
            hid.DeviceRemoved += Hid_DeviceRemoved;

            ptr = new IntPtr(-1);

            Util.Output = (obj) => outputBox.Text += obj.ToString();
        }

        private void btnRun_Click(object sender, EventArgs e)
        {
            outputBox.Clear();
            SimCore core = SimCore.GetSimCore(codeBox.Text);
            var result = core.Run();
            if (result != null)
            {
                byte[][] bytes = core.GetBytes();
                outputBox.Text += "Code: ";
                for (int i = 0; i < bytes.Length; i++)
                    for (int j = 0; j < bytes[i].Length; j++)
                        outputBox.Text += (bytes[i][j].ToString("x2") + " ");
                outputBox.Text += "\n";

                if ((int)ptr != -1)
                {
                    outputBox.Text += "Sending ...\n";

                    Report report; int index = 0;
                    List<byte> buf = new List<byte>();
                    while (index < bytes.Length)
                    {
                        if (buf.Count + bytes[index].Length < 16)
                        {
                            buf.AddRange(bytes[index]);
                            index += 1;
                            if (index >= bytes.Length)
                            {
                                report = new Report(0x55, buf.ToArray());
                                hid.Write(report);
                                buf.Clear();
                                break;
                            }
                        }
                        else
                        {
                            report = new Report(0x55, buf.ToArray());
                            hid.Write(report);
                            buf.Clear();
                        }
                    }
                }
            }
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (btnConnect.Text == "Connect")
            {
                ushort vid = 0x8088; byte mid = 0x02;
                ushort pid = (ushort)(devList.SelectedIndex + 1);
                if (devList.SelectedItem.Equals("SimPad Boot"))
                    pid = 0x00FF;
                ptr = hid.OpenDevice(vid, pid, mid);
                if ((int)ptr != -1)
                {
                    outputBox.Clear();
                    outputBox.Text += ("Connected to: " + devList.SelectedItem + "\n");
                    devList.Enabled = false;
                    btnConnect.Text = "Close";
                }
            }
            else
            {
                if ((int)ptr != -1)
                    hid.CloseDevice(ptr);
                ptr = new IntPtr(-1);
                devList.Enabled = true;
                btnConnect.Text = "Connect";
            }
        }

        private void Hid_DeviceRemoved(object sender, EventArgs e)
        {
            Invoke(new ThreadStart(() => {
                ptr = new IntPtr(-1);
                devList.Enabled = true;
                btnConnect.Text = "Connect";
            }));
        }

        private void Hid_DataReceived(object sender, Report e)
        {
            byte[] bytes = e.reportBuff;
            Invoke(new ThreadStart(() => {
                outputBox.Text += "Data: ";
                for (int i = 0; i < bytes.Length; i++)
                    outputBox.Text += (bytes[i].ToString("x2") + " ");
                outputBox.Text += "\n";
            }));
        }

        private void statusLabel_Click(object sender, EventArgs e)
        {
            new HelpForm().Show();
        }
    }
}
