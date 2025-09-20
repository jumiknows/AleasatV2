import type { Request } from "express";
import type { ParamsDictionary } from "express-serve-static-core";

export const getAccessTokenFromHeader = (
  req: Request<ParamsDictionary, any, any, Record<string, any>>,
): string | undefined => {
  const authHeader = req.headers.authorization;
  if (!authHeader) return undefined;

  const match = authHeader.match(/Bearer (.+)/);
  if (match === null) return undefined;

  const accessToken = match[1];
  if (!accessToken) return undefined;
  return accessToken;
};
