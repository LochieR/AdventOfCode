using System.Text;

namespace AoC23
{
    public static class DayTwo
    {
        public static async Task Run()
        {
            FileStream fs = new FileStream("inputs/Day2.txt", FileMode.Open, FileAccess.Read);
            
            using var reader = new StreamReader(fs);
            string input = await reader.ReadToEndAsync();

            PartOneAndTwo(input);
        }

        private static void PartOneAndTwo(string input)
        {
            string[] lines = input.Split("\n", StringSplitOptions.RemoveEmptyEntries)
                             .Select(line => line.Trim())
                             .ToArray();

            List<int> possibleGames = new List<int>();
            List<int> powers = new List<int>();

            int redCount = 12;
            int greenCount = 13;
            int blueCount = 14;

            foreach (var line in lines)
            {
                var parts = line.Split(": ");
                var gameInfo = parts[1].Split("; ").ToList();
                int gameNum = int.Parse(parts[0].Split(' ')[1]);

                int power;

                if (IsGamePossible(gameInfo, redCount, greenCount, blueCount, out power))
                {
                    possibleGames.Add(gameNum);
                }

                powers.Add(power);
            }

            int total = possibleGames.Sum();
            int powersSum = powers.Sum();
            Console.WriteLine($"Total game IDs for part 1: {total}");
            Console.WriteLine($"Total powers for part 2: {powersSum}");
        }

        static bool IsGamePossible(List<string> gameInfo, int redCount, int greenCount, int blueCount, out int power)
        {
            int maxRed = 0, maxGreen = 0, maxBlue = 0;

            power = 0;

            bool possible = true;

            foreach (var subset in gameInfo)
            {
                var cubeCounts = subset.Split(", ");
                int redSubset = 0, greenSubset = 0, blueSubset = 0;

                foreach (var count in cubeCounts)
                {
                    var parts = count.Split(' ');
                    int countNum = int.Parse(parts[0]);
                    string colour = parts[1];

                    if (colour == "red")
                    {
                        redSubset += countNum;

                        if (countNum > maxRed)
                            maxRed = countNum;
                    }
                    else if (colour == "green")
                    {
                        greenSubset += countNum;
                        
                        if (countNum > maxGreen)
                            maxGreen = countNum;
                    }
                    else if (colour == "blue")
                    {
                        blueSubset += countNum;

                        if (countNum > maxBlue)
                            maxBlue = countNum;
                    }
                }

                power = maxRed * maxGreen * maxBlue;

                if (redSubset > redCount || greenSubset > greenCount || blueSubset > blueCount)
                    possible = false;
            }
            power = maxRed * maxGreen * maxBlue;

            return possible;
        }
    }
}