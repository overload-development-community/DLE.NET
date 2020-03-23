using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    [System.ComponentModel.DesignerCategory("Code")]
    public class RepeatingButton : Button
    {
        private const int INITIAL_DELAY = 500;
        private const int TICK_RATE_SLOW = 107;
        private const int MAX_SLOW_TICKS = 14;
        private const int TICK_RATE_FAST = 20;
        private Timer timer;
        public new event EventHandler Click;

        private bool pastInitialDelay = false;
        private int slowTicks = 0;

        public RepeatingButton() : base()
        {
            this.timer = new Timer();
            this.MouseDown += RepeatingButton_MouseDown;
            this.MouseUp += RepeatingButton_MouseUp;
            timer.Tick += Timer_Tick;
        }

        public new void PerformClick()
        {
            Click?.Invoke(this, new EventArgs());
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            if (!pastInitialDelay)
            {
                pastInitialDelay = true;
                slowTicks = 1;
                timer.Interval = TICK_RATE_SLOW;
            }
            else if (slowTicks < MAX_SLOW_TICKS)
            {
                ++slowTicks;
            }
            else
            {
                timer.Interval = TICK_RATE_FAST;
            }
            this.PerformClick();
        }

        private void RepeatingButton_MouseDown(object sender, MouseEventArgs e)
        {
            this.PerformClick();
            pastInitialDelay = false;
            slowTicks = 0;
            timer.Interval = INITIAL_DELAY;
            timer.Start();
        }

        private void RepeatingButton_MouseUp(object sender, MouseEventArgs e)
        {
            timer.Stop();
        }
    }
}
