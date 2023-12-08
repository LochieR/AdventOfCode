using AoC.Shared;

using AoC23;

namespace AoC
{
    public class Advent
    {
        public readonly ConsoleColor DefaultConsoleColor = Console.ForegroundColor;

        private int _day = 0;

        public Advent()
        {
            Console.OutputEncoding = System.Text.Encoding.Unicode;

            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("████████████████");
            Console.Write("██████");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write("███████");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("██");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("█");
            Console.Write("████");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write("████████");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.Write("█");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("██");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("█");
            Console.Write("███");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write("██████████");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("███");
            Console.Write("██");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("████████████");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("██");
            Console.Write("██\x1b[38;5;210m█\x1b[38;5;216m██");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("█");
            Console.ForegroundColor = ConsoleColor.Blue;
            Console.Write("█\x1b[38;5;216m██");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("█");
            Console.ForegroundColor = ConsoleColor.Blue;
            Console.Write("█\x1b[38;5;216m██\x1b[38;5;210m█");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("██");
            Console.Write("███\x1b[38;5;216m████\x1b[38;5;210m██\x1b[38;5;216m████");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("███");
            Console.Write("███");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("██████████");
            Console.ForegroundColor = ConsoleColor.Gray;
            Console.WriteLine("███");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write("████");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("████████");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("████");
            Console.Write("█████");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("██████");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("█████");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write("███████");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write("██");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("███████");

            Console.ForegroundColor = DefaultConsoleColor;

            Console.WriteLine("The Advent of Code 2023. Enter a day:");
        }

        public void SelectDay()
        {
            int number = 0;

            while (number == 0)
            {
                try
                {
                    number = int.Parse(Console.ReadLine() ?? "");

                    if (number < 1 || number > 25)
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Enter a valid number between 1 and 25.");
                        Console.ForegroundColor = DefaultConsoleColor;
                        number = 0;
                    }
                }
                catch
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Enter a valid number between 1 and 25.");
                    Console.ForegroundColor = DefaultConsoleColor;
                    number = 0;
                }
            }

            _day = number;
        }

        public async Task Run()
        {
            Timer timer = new Timer();

            switch (_day)
            {
                case 1:
                    Console.WriteLine("Running Advent of Code day 1.");
                    await DayOne.Run();
                    break;
                case 2:
                    Console.WriteLine("Running Advent of Code day 2.");
                    await DayTwo.Run();
                    break;
                case 3:
                    Console.WriteLine("Running Advent of Code day 3.");
                    await DayThree.Run();
                    break;
                case 4:
                    Console.WriteLine("Running Advent of Code day 4.");
                    await DayFour.Run();
                    break;
                case 5:
                    Console.WriteLine("Running Advent of Code day 5.");
                    await DayFive.Run();
                    break;
            }

            timer.Stop();
            Console.WriteLine($"Elapsed time: {timer}");
        }
    }
}
