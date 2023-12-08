using System.Text;

namespace AoC23
{
    public static class DayThree
    {
        public static async Task Run()
        {
            FileStream fs = new FileStream("inputs/Day3.txt", FileMode.Open, FileAccess.Read);
            
            using var reader = new StreamReader(fs);
            string input = await reader.ReadToEndAsync();

            PartOneAndTwo(input);
        }


        private static void PartOneAndTwo(string input)
        {
            string[] lines = input.Split(Environment.NewLine);
            List<List<char>> table = (from line in lines
                                      select line.ToList()).ToList();
            Dictionary<(int, int), List<int>> symbolDict = [];
            int maxr = table.Count;
            int maxc = table[0].Count;
            int[][] offsets =
            [
                [-1, -1], [-1, 0], [-1, 1],
                [0, -1], [0, 1],
                [1, -1], [1, 0], [1, 1]
            ];

            int num = 0;
            HashSet<(int, int)> neighborSymbols = [];

            for (int r = 0; r < maxr; r++)
            {
                for (int c = 0; c < maxc; c++)
                {
                    char ch = table[r][c];
                    if (char.IsDigit(ch))
                    {
                        foreach (int[] offset in offsets)
                        {
                            int dr = offset[0];
                            int dc = offset[1];
                            if (r + dr >= 0 && r + dr < maxr && c + dc >= 0 && c + dc < maxc
                                && !char.IsDigit(table[r + dr][c + dc]) && table[r + dr][c + dc] != '.')
                            {
                                neighborSymbols.Add((r + dr, c + dc));
                            }
                        }

                        num = num * 10 + (int)char.GetNumericValue(ch);
                        continue;
                    }

                    if (num != 0 && neighborSymbols.Count != 0)
                    {
                        foreach ((int sr, int sc) in neighborSymbols)
                        {
                            if (!symbolDict.ContainsKey((sr, sc)))
                            {
                                symbolDict[(sr, sc)] = [];
                            }
                            symbolDict[(sr, sc)].Add(num);
                        }
                    }

                    num = 0;
                    neighborSymbols.Clear();
                }
            }

            int sumOfNums = (from pair in symbolDict
                            where !char.IsDigit(table[pair.Key.Item1][pair.Key.Item2])
                            from x in pair.Value
                            select x).Sum();
            int sumOfProducts = (from pair in symbolDict
                                 where table[pair.Key.Item1][pair.Key.Item2] == '*' && pair.Value.Count == 2
                                 select pair.Value[0] * pair.Value[1]).Sum();

            Console.WriteLine(sumOfNums);
            Console.WriteLine(sumOfProducts);
        }
    }
}