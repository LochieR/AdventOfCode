using AoC.Shared;

namespace AoC23
{
    using SeedMap = (long DestinationStart, long SourceStart, long Range);

    public static class DayFive
    {
        public static async Task Run()
        {
            FileStream fs = new FileStream("inputs/Day5.txt", FileMode.Open, FileAccess.Read);

            using var reader = new StreamReader(fs);
            string input = await reader.ReadToEndAsync();

            PartOne(input);
            PartTwo(input);
        }

        private static void PartOne(string input)
        {
            string[] lines = input.Split(Environment.NewLine);

            List<long> seeds = [];

            string[] seedsSplit = lines[0].Split(": ", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
            string[] seedsStrSplit = seedsSplit[1].Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);

            seeds = (from seed in seedsStrSplit select long.Parse(seed)).ToList();

            lines = [.. lines.RemoveFromStart(2)];

            Console.WriteLine(GetLocation(seeds, lines));
        }

        private static void PartTwo(string input)
        {
            string[] lines = input.Split(Environment.NewLine);

            List<long> seeds = [];

            string[] seedsSplit = lines[0].Split(": ", StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
            string[] seedsStrSplit = seedsSplit[1].Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);

            List<long> seedPairs = [];
            List<(long seedStart, long range)> seedRangePairs = [];

            foreach (string seed in seedsStrSplit)
            {
                seedPairs.Add(long.Parse(seed));
            }

            if (seedPairs.Count % 2 != 0)
                throw new Exception();

            for (int i = 0; i < seedPairs.Count; i += 2)
            {
                seedRangePairs.Add((seedPairs[i], seedPairs[i + 1]));
            }

            List<long> locations = [];

            lines = [.. lines.RemoveFromStart(2)];

            int x = 0;
            foreach (var pair in seedRangePairs)
            {
                for (int i = 0; i < pair.range; i++)
                {
                    seeds.Add(pair.seedStart + i);
                }

                Console.SetCursorPosition(0, Console.GetCursorPosition().Top);
                Console.Write($"{(float)x / seedRangePairs.Count}%       ");

                locations.Add(GetLocation(seeds, lines));
                seeds.Clear();
                x++;
            }
            Console.WriteLine();

            Console.WriteLine(locations.Min());
        }

        private static long GetLocation(List<long> seeds, string[] lines)
        {
            List<List<SeedMap>> maps = [];

            int currentMap = 0;
            foreach (string line in lines)
            {
                if (maps.Count == 0)
                    maps.Add([]);

                if (string.IsNullOrWhiteSpace(line) || string.IsNullOrEmpty(line))
                {
                    maps.Add([]);
                    currentMap++;
                    continue;
                }

                if (line.EndsWith(':'))
                    continue;

                string[] values = line.Split(' ', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
                try
                {
                    maps[currentMap].Add(new SeedMap(long.Parse(values[0]), long.Parse(values[1]), long.Parse(values[2])));
                }
                catch
                {
                    Console.WriteLine(line);
                    throw;
                }
            }

            List<long> currentValues = new List<long>(seeds.Count);
            seeds.ForEach(currentValues.Add);

            List<long> tempValues = [];

            for (int i = 0; i < maps.Count; i++)
            {
                List<SeedMap> map = maps[i];

                foreach (long value in currentValues)
                {
                    bool mapped = false;

                    foreach (SeedMap sm in map)
                    {
                        if (value >= sm.SourceStart && value <= sm.SourceStart + (sm.Range - 1))
                        {
                            tempValues.Add(sm.DestinationStart + (value - sm.SourceStart));
                            mapped = true;
                            break;
                        }
                    }

                    if (!mapped)
                        tempValues.Add(value);
                }

                currentValues.Clear();
                tempValues.ForEach(currentValues.Add);
                tempValues.Clear();
            }

            return currentValues.Min();
        }
    }
}
