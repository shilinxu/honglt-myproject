using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Data;
using System.Data.SqlClient;

namespace DataCenter
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            AppDomain.CurrentDomain.SetData("APP_CONFIG_FILE", "app.config");
            SplashScreen.Setup(typeof(frmSplash));

            SplashScreen.Status = "注册表";
            Properties.Settings.Synchronized(new Properties.Settings());

            SplashScreen.Status = "数据库";

            string connect = "";// "Provider=SQLOLEDB.1;";
            connect += "Persist Security Info=False;";
            connect += "User ID=" + Properties.Settings.Database.UserId + ";";
            connect += "Password=" + Properties.Settings.Database.Password + ";";
            connect += "Initial Catalog=" + Properties.Settings.Database.Database + ";";
            connect += "Data Source=" + Properties.Settings.Database.Host;

            Database.SQLConnection = Database.Open(connect);

            SplashScreen.Status = " 图形用户界面";
            frmMain frmMain = new frmMain();

            SplashScreen.Close();
            Application.Run(frmMain);
        }
    }

    internal abstract class Database
    {
        static public SqlConnection SQLConnection;

        static public SqlConnection Open(string connect)
        {
            try
            {
                SQLConnection = new SqlConnection(connect);
                SQLConnection.Open(); return SQLConnection;
            }
            catch (Exception)
            {
                if (SQLConnection != null) SQLConnection.Dispose();
                return null;
            }
        }
    }
}