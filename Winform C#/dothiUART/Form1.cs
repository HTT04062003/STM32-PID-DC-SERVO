using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using ZedGraph;
using System.IO.Ports;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Rebar;
using System.Runtime.InteropServices.ComTypes;
using static System.Net.Mime.MediaTypeNames;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.TaskbarClock;

namespace dothiUART
{
    public partial class Form1 : Form
    {
        
        public Form1()
        {
            InitializeComponent();
        }
        string[] baud = { "9600", "14400" };
        string[] databit = { "6", "7", "8" };
        int speed_mode = 0;
        int pos_mode = 0;
        private void Form1_Load(object sender, EventArgs e)
        {
                GraphPane mypane = zedGraphControl1.GraphPane;
            
                mypane.Title.Text = "DC SERVO RESPONSE";
                mypane.XAxis.Title.Text = "Time";
                mypane.YAxis.Title.Text = "Value";

                RollingPointPairList list1 = new RollingPointPairList(6000000);
                LineItem duongline1 = mypane.AddCurve("Speed", list1, Color.Blue, SymbolType.None);

            mypane.Chart.Fill = new Fill(Color.White, Color.LightGray, 90.0f);

            zedGraphControl1.AxisChange();
            
            

            string[] myport = SerialPort.GetPortNames();
            comboBox1.Items.AddRange(myport);
            comboBox2.Items.AddRange(baud);
            comboBox3.Items.AddRange(databit);
            comboBox4.Items.AddRange(Enum.GetNames(typeof(Parity)));

            
        }
        double tong = 0;
        private void draw(double line1)
        {
            LineItem duongline1 = zedGraphControl1.GraphPane.CurveList[0] as LineItem;
            if (duongline1 == null)
                return;
            IPointListEdit list1 = duongline1.Points as IPointListEdit;
            if (list1 == null)
                return;
            list1.Add(tong, line1);
            
            zedGraphControl1.AxisChange();
            zedGraphControl1.Invalidate();
            tong += 0.01    ;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort1.PortName = comboBox1.Text;
                serialPort1.BaudRate = int.Parse(comboBox2.Text);
                serialPort1.DataBits = int.Parse(comboBox3.Text);
                serialPort1.Parity = (Parity)Enum.Parse(typeof(Parity), comboBox4.Text);
                serialPort1.StopBits = (StopBits)Enum.Parse(typeof(StopBits), comboBox5.Text);
                serialPort1.Open();
                button1.Enabled = false;
                button2.Enabled = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        int i = 0;
        string[] Buffer;
        
        int iData;
        

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            string data = "";
            string chuoi;
            iData += 1;
            data = serialPort1.ReadLine();
            chuoi = Regex.Match(data, @"\d+").Value;
            iData = int.Parse(chuoi);
                
            Invoke(new MethodInvoker(() => listBox1.Items.Add(chuoi)));
            Invoke(new MethodInvoker(() => draw(iData)));

















        }
        double cnt = 10;
        private void button3_Click(object sender, EventArgs e)
        {
            
            serialPort1.Write("S");
            serialPort1.Write(textBox6.Text);
            serialPort1.Write(";");


        }

        private void button2_Click(object sender, EventArgs e)
        {
            serialPort1.Close();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void button7_Click(object sender, EventArgs e)
        {
            
            serialPort1.Write(textBox10.Text);
            serialPort1.Write(";");
        }

        private void button4_Click(object sender, EventArgs e)
        {
            string tmp = "P";
            tmp += textBox7.Text;
            tmp += '.';
            serialPort1.Write(tmp);

        }

        private void button5_Click(object sender, EventArgs e)
        {
            string tmp = "I";
            tmp += textBox7.Text;
            tmp += '.';
            serialPort1.Write(tmp);

        }

        private void textBox9_TextChanged(object sender, EventArgs e)
        {
            string tmp = "D";
            tmp += textBox7.Text;
            serialPort1.Write(tmp);
        }

        private void button8_Click(object sender, EventArgs e)
        {
            textBox7.Text = "0.173";
            textBox8.Text = "5.31";
            textBox9.Text = "0";
        }

        private void button9_Click(object sender, EventArgs e)
        {
            textBox7.Text = "3.2";
            textBox8.Text = "3.3";
            textBox9.Text = "0.16";
        }
    }
}
