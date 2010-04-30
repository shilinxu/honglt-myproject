using System;
using System.Threading;
using System.Reflection;
using System.Windows.Forms;

namespace DataCenter
{
    public partial class frmSplash : Form, ISplashForm
    {
        public frmSplash()
        {
            InitializeComponent();
        }

        #region ISplashForm

        void ISplashForm.SetStatusInfo(string status)
        {
            lblStatus.Text = status;
        }

        #endregion

        private void frmSplash_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Alt && e.KeyCode == Keys.F4)
            {
                e.Handled = true;
            }
        }
    }
    
    public partial class SplashScreen
    {
        private static frmSplash m_frmSplash;
        private static ISplashForm m_pSplashInterface;
        private static Thread m_pThread = null;
        
        /// <summary>
        /// Show the SplashForm
        /// </summary>
        public static bool Setup(Type formType)
        {
            if (m_pThread != null) return false;

            m_pThread = new Thread(new ThreadStart(delegate()
            {
                CreateInstance(formType);
                Application.Run(m_frmSplash);
            }));

            m_pThread.IsBackground = true;
            m_pThread.SetApartmentState(ApartmentState.STA);
            m_pThread.Start();

            return true;
        }

        /// <summary>
        /// Colse the SplashForm
        /// </summary>
        public static void Close()
        {
            if (m_pThread == null) return;

            try
            {
                if (m_frmSplash != null)
                    m_frmSplash.Invoke(new MethodInvoker(m_frmSplash.Close));
            }
            catch (Exception)
            {
            }
            m_pThread.Abort();
            m_pThread = null;
            m_frmSplash = null;
        }

        /// <summary>
        /// set the loading Status
        /// </summary>
        public static string Status
        {
            set
            {
                if (m_pSplashInterface == null || m_frmSplash == null) return;
                
                m_frmSplash.Invoke(
                        new SplashStatusChangedHandle(delegate(string str) { m_pSplashInterface.SetStatusInfo(str); }),
                        new object[] { value }
                    );
            }
        }

        private static void CreateInstance(Type FormType)
        {

            object obj = FormType.InvokeMember(null,
                                BindingFlags.DeclaredOnly |
                                BindingFlags.Public | BindingFlags.NonPublic |
                                BindingFlags.Instance | BindingFlags.CreateInstance, null, null, null);
            m_frmSplash = obj as frmSplash;
            m_pSplashInterface = obj as ISplashForm;
            if (m_frmSplash == null)
            {
                throw (new Exception("Splash Screen must inherit from System.Windows.Forms.Form"));
            }
            if (m_pSplashInterface == null)
            {
                throw (new Exception("must implement interface ISplashForm"));
            }
        }
        
        private delegate void SplashStatusChangedHandle(string NewStatusInfo);
    }

    /// <summary>
    /// interface for Splash Screen
    /// </summary>
    public interface ISplashForm
    {
        void SetStatusInfo(string NewStatusInfo);
    }
}