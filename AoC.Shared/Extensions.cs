namespace AoC.Shared
{
    public static class Extensions
    {
        public static TCollection RemoveIndex<TCollection, T>(this TCollection collection, int index) where TCollection : ICollection<T>, new()
        {
            T[] array = new T[collection.Count - 1];

            int i = 0;
            foreach (var item in collection)
            {
                if (i != index)
                    array[i] = item;
                i++;
            }

            return [.. array];
        }

        public static ICollection<T> RemoveFromStart<T>(this ICollection<T> collection, int count = 1)
        {
            T[] array = new T[collection.Count - count];

            for (int i = 0, x = 0; i < collection.Count;)
            {
                if (i >= count)
                {
                    array[x] = collection.ElementAt(i);
                    x++;
                }
                i++;
            }

            return [.. array];
        }
    }
}
