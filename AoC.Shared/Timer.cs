using System.Diagnostics;

namespace AoC.Shared
{
    public class Timer
    {
        private Stopwatch _stopwatch;

        public TimeSpan Elapsed
        {
            get
            {
                return _stopwatch.Elapsed;
            }
        }

        public Timer()
        {
            _stopwatch = new Stopwatch();
            _stopwatch.Start();
        }

        public void Stop()
        {
            _stopwatch.Stop();
        }

        public override string ToString()
        {
            return $"{Elapsed.Minutes}m {Elapsed.Seconds}s {Elapsed.Milliseconds}ms";
        }

        public string ToString(string? format)
        {
            return Elapsed.ToString(format);
        }

        public string ToString(string? format, IFormatProvider? formatProvider)
        {
            return Elapsed.ToString(format, formatProvider);
        }
    }
}
