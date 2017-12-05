using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace WpfApp
{
    public class App : System.Windows.Application
    {
        [STAThread]
        static void Main(string []args)
        {
            App app = new App();
            WpfApp.MainWindow mainWindow = new MainWindow();
            app.Run(mainWindow);
            File.WriteAllText("t.txt", "plop");
        }
    }
}
