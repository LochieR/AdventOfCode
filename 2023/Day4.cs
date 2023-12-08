namespace AoC23
{
    public static class DayFour
    {
        public static async Task Run()
        {
            FileStream fs = new FileStream("inputs/Day4.txt", FileMode.Open, FileAccess.Read);
            
            using var reader = new StreamReader(fs);
            string input = await reader.ReadToEndAsync();

            PartOne(input);
            PartTwo(input);
        }

        private static void PartOne(string input)
        {
            string[] lines = input.Split(Environment.NewLine);

            int total = 0;

            foreach (string line in lines)
            {
                List<int> winningNumbers = [];
                List<int> numbers = [];

                int cardID = int.Parse(line.Split(": ", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)[0].Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)[1]);
                string numbersOnly = line.Split(": ", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)[1];

                string[] winningNumbersSplit = numbersOnly.Split('|', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
                string winningNumbersStr = winningNumbersSplit[0].Trim();
                string numbersStr = winningNumbersSplit[1].Trim();

                winningNumbers = (from num in winningNumbersStr.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
                                  select int.Parse(num)).ToList();
                numbers = (from num in numbersStr.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
                           select int.Parse(num)).ToList();

                List<int> wins = winningNumbers.Intersect(numbers).ToList();

                if (wins.Count > 0)
                    total += (int)MathF.Pow(2, wins.Count - 1);
            }

            Console.WriteLine($"Total points scored for part 1: {total}");
        }
    
        private static void PartTwo(string input)
        {
            string[] lines = input.Split(Environment.NewLine);
            
            int numCards = 0;
            int[] cards = new int[lines.Length];
            Array.Fill(cards, 1);

            int i = 0;
            foreach (string line in lines)
            {
                for (int x = 0; x < cards[i]; x++)
                {
                    List<int> winningNumbers = [];
                    List<int> numbers = [];

                    string numbersOnly = line.Split(": ", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)[1];
                    string[] winningNumbersSplit = numbersOnly.Split('|', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
                    string winningNumbersStr = winningNumbersSplit[0].Trim();
                    string numbersStr = winningNumbersSplit[1].Trim();

                    winningNumbers = (from num in winningNumbersStr.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
                                    select int.Parse(num)).ToList();
                    numbers = (from num in numbersStr.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
                            select int.Parse(num)).ToList();

                    int numWins = winningNumbers.Intersect(numbers).Count();

                    for (int j = 0; j < numWins; j++)
                    {
                        cards[i + 1 + j] += 1;
                    }
                }

                i++;
            }

            numCards = cards.Sum();

            Console.WriteLine(numCards);
        }
    }
}