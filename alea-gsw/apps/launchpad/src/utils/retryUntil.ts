export const retryUntilSuccess = async <T>(
  fn: () => Promise<T>,
  maxRetry = 10,
  waitMs = 1000,
): Promise<T | undefined> => {
  let retryCount = 0;

  while (retryCount < maxRetry) {
    try {
      return await fn();
    } catch {
      retryCount++;
      await new Promise((resolve) => setTimeout(resolve, waitMs));
    }
  }

  return undefined;
};

export const retryUntilCondition = async <T>(
  fn: () => Promise<T>,
  condition: (result: T) => boolean,
  maxRetry = 10,
  waitMs = 1000,
): Promise<T | undefined> => {
  let retryCount = 0;

  while (retryCount < maxRetry) {
    try {
      const result = await fn();
      if (condition(result)) {
        return result;
      }
    } catch {
      retryCount++;
      await new Promise((resolve) => setTimeout(resolve, waitMs));
    }
  }

  return undefined;
};
