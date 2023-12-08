namespace AoC23 
{
    public static class DayOne
    {
        public static async Task Run()
        {
            FileStream fs = new FileStream("inputs/Day1.txt", FileMode.Open, FileAccess.Read);
            
            using var reader = new StreamReader(fs);
            string input = await reader.ReadToEndAsync();

            PartOne(input);
            PartTwo(input);
        }

        private static void PartOne(string input)
        {
            string[] lines = input.Split('\n');

            int total = 0;

            foreach (string line in lines)
            {
                List<int> numbers = new List<int>();

                foreach (char c in line)
                {
                    if (c >= '0' && c <= '9')
                        numbers.Add(Convert.ToInt32(new string(c, 1)));
                }

                int first = numbers.First();
                int last = numbers.Last();

                int calibrationValue = Convert.ToInt32($"{first}{last}");

                total += calibrationValue;
            }

            Console.WriteLine($"Total of calibration values for part 1: {total}");
        }

        private static void PartTwo(string input)
        {
            string[] lines = input.Split('\n');

            int total = 0;

            foreach (string line in lines)
            {
                List<int> numbers = new List<int>();

                int i = 0;
                foreach (char c in line)
                {
                    bool valid = CheckNumber(c, i, line, out int number);

                    if (valid)
                        numbers.Add(number);

                    i++;
                }

                int first = numbers.First();
                int last = numbers.Last();

                int calibrationValue = Convert.ToInt32($"{first}{last}");

                total += calibrationValue;
            }

            Console.WriteLine($"Total of calibration values for part 2: {total}");
        }

        private static bool CheckNumber(char c, int index, string line, out int value)
        {
            var checkZero = (char ch1, char ch2, char ch3, char ch4) =>
            {
                if (ch1 == 'z' && ch2 == 'e' && ch3 == 'r' && ch4 == 'o')
                    return true;

                return false;
            };
            
            var checkOne = (char ch1, char ch2, char ch3) =>
            {
                if (ch1 == 'o' && ch2 == 'n' && ch3 == 'e')
                    return true;

                return false;
            };

            var checkTwo = (char ch1, char ch2, char ch3) =>
            {
                if (ch1 == 't' && ch2 == 'w' && ch3 == 'o')
                    return true;

                return false;
            };

            var checkThree = (char ch1, char ch2, char ch3, char ch4, char ch5) =>
            {
                if (ch1 == 't' && ch2 == 'h' && ch3 == 'r' && ch4 == 'e' && ch5 == 'e')
                    return true;

                return false;
            };

            var checkFour = (char ch1, char ch2, char ch3, char ch4) =>
            {
                if (ch1 == 'f' && ch2 == 'o' && ch3 == 'u' && ch4 == 'r')
                    return true;

                return false;
            };

            var checkFive = (char ch1, char ch2, char ch3, char ch4) =>
            {
                if (ch1 == 'f' && ch2 == 'i' && ch3 == 'v' && ch4 == 'e')
                    return true;

                return false;
            };

            var checkSix = (char ch1, char ch2, char ch3) =>
            {
                if (ch1 == 's' && ch2 == 'i' && ch3 == 'x')
                    return true;

                return false;
            };

            var checkSeven = (char ch1, char ch2, char ch3, char ch4, char ch5) =>
            {
                if (ch1 == 's' && ch2 == 'e' && ch3 == 'v' && ch4 == 'e' && ch5 == 'n')
                    return true;

                return false;
            };

            var checkEight = (char ch1, char ch2, char ch3, char ch4, char ch5) =>
            {
                if (ch1 == 'e' && ch2 == 'i' && ch3 == 'g' && ch4 == 'h' && ch5 == 't')
                    return true;

                return false;
            };

            var checkNine = (char ch1, char ch2, char ch3, char ch4) =>
            {
                if (ch1 == 'n' && ch2 == 'i' && ch3 == 'n' && ch4 == 'e')
                    return true;

                return false;
            };

            if (line.Substring(index).Length >= 4 && checkZero(c, line[index + 1], line[index + 2], line[index + 3]))
            {
                value = 0;
                return true;
            }

            if (line.Substring(index).Length >= 3 && checkOne(c, line[index + 1], line[index + 2]))
            {
                value = 1;
                return true;
            }

            if (line.Substring(index).Length >= 3 && checkTwo(c, line[index + 1], line[index + 2]))
            {
                value = 2;
                return true;
            }

            if (line.Substring(index).Length >= 5 && checkThree(c, line[index + 1], line[index + 2], line[index + 3], line[index + 4]))
            {
                value = 3;
                return true;
            }

            if (line.Substring(index).Length >= 4 && checkFour(c, line[index + 1], line[index + 2], line[index + 3]))
            {
                value = 4;
                return true;
            }

            if (line.Substring(index).Length >= 4 && checkFive(c, line[index + 1], line[index + 2], line[index + 3]))
            {
                value = 5;
                return true;
            }

            if (line.Substring(index).Length >= 3 && checkSix(c, line[index + 1], line[index + 2]))
            {
                value = 6;
                return true;
            }

            if (line.Substring(index).Length >= 5 && checkSeven(c, line[index + 1], line[index + 2], line[index + 3], line[index + 4]))
            {
                value = 7;
                return true;
            }

            if (line.Substring(index).Length >= 5 && checkEight(c, line[index + 1], line[index + 2], line[index + 3], line[index + 4]))
            {
                value = 8;
                return true;
            }

            if (line.Substring(index).Length >= 4 && checkNine(c, line[index + 1], line[index + 2], line[index + 3]))
            {
                value = 9;
                return true;
            }
            
            if (c >= '0' && c <= '9')
            {
                value = Convert.ToInt32(new string(c, 1));
                return true;
            }

            value = -1;
            return false;
        }
    }
}