using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    [DesignerCategory("Code")]
    [Description("A button that repeats click events while it is held.")]
    public class RepeatingButton : Button
    {
        private const int INITIAL_DELAY = 500;
        private const int TICK_RATE_SLOW = 107;
        private const int MAX_SLOW_TICKS = 14;
        private const int TICK_RATE_FAST = 20;
        private Timer timer;
        private bool timerFromMouse = false;
        public new event EventHandler Click;
        private Keys lastKeys;

        public bool ClickOnlyOnUp { get; set; } = false;

        private bool pastInitialDelay = false;
        private int slowTicks = 0;

        public RepeatingButton() : base()
        {
            timer = new Timer();
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
            PerformClick();
        }

        private void StartTimer(bool mouse)
        {
            pastInitialDelay = false;
            slowTicks = 0;
            timer.Interval = INITIAL_DELAY;
            timer.Start();
            timerFromMouse = mouse;
        }

        private void StopTimer(bool mouse)
        {
            if (mouse || !timerFromMouse)
            {
                timer.Stop();
                timerFromMouse = false;
            }
        }

        protected override void OnEnabledChanged(EventArgs e)
        {
            base.OnEnabledChanged(e);
            if (!this.Enabled)
                StopTimer(true);
        }

        protected override void OnMouseDown(MouseEventArgs mevent)
        {
            if (this.Enabled)
            {
                if (!ClickOnlyOnUp) PerformClick();
                StartTimer(true);
            }
            base.OnMouseDown(mevent);
        }

        protected override void OnMouseUp(MouseEventArgs mevent)
        {
            if (this.Enabled && ClickOnlyOnUp && this.ClientRectangle.Contains(mevent.Location)) PerformClick();
            StopTimer(true);
            base.OnMouseUp(mevent);
        }

        protected override void OnPreviewKeyDown(PreviewKeyDownEventArgs e)
        {
            base.OnPreviewKeyDown(e);
            if (e.KeyCode == Keys.Enter)
            {
                e.IsInputKey = true;
            }
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            if (timerFromMouse) StopTimer(true);
            base.OnMouseLeave(e);
        }

        protected override void OnMouseMove(MouseEventArgs mevent)
        {
            if (timer.Enabled && timerFromMouse && !this.ClientRectangle.Contains(mevent.Location))
            {
                StopTimer(true);
            }
            base.OnMouseMove(mevent);
        }

        protected override void OnKeyDown(KeyEventArgs kevent)
        {
            if (this.Enabled && (kevent.KeyCode == Keys.Space || kevent.KeyCode == Keys.Enter))
            {
                if (lastKeys != kevent.KeyCode)
                    StartTimer(false);
                lastKeys = kevent.KeyCode;
            }
            base.OnKeyDown(kevent);
        }

        protected override void OnKeyUp(KeyEventArgs kevent)
        {
            if (kevent.KeyCode == lastKeys)
            {
                StopTimer(false);
            }
            base.OnKeyUp(kevent);
        }
    }
}
