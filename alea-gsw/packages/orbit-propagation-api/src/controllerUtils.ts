import type { Request, Response } from "express";
import type { ParamsDictionary, Query } from "express-serve-static-core";
import type { components, operations } from "./generated/schema/openapi.js";

export type ServiceReturnType<T extends keyof operations & string> =
  | {
      // @ts-expect-error it can
      data: operations[T]["responses"]["200"]["content"]["application/json"];
      error?: undefined;
    }
  | {
      data?: undefined;
      error: components["schemas"]["Problem"];
    };

export type ServiceReturnTypeV2<T extends keyof operations & string> =
  | {
      success: true;
      // @ts-expect-error it can
      data: operations[T]["responses"]["200"]["content"]["application/json"];
    }
  | {
      success: false;
      error: components["schemas"]["Problem"];
    };

type NonUndefined<T> = T extends undefined ? never : T;

export type OperationsRequest<T extends keyof operations & string> = Request<
  operations[T] extends { parameters: { path: any } }
    ? operations[T]["parameters"]["path"]
    : ParamsDictionary,
  any,
  operations[T] extends {
    requestBody: { content: { "application/json": any } };
  }
    ? operations[T]["requestBody"]["content"]["application/json"]
    : any,
  operations[T] extends {
    parameters: {
      query?: Record<string, any>;
    };
  }
    ? NonUndefined<operations[T]["parameters"]["query"]>
    : Query
>;

export const responseWithError = async (
  req: Request<ParamsDictionary, any, any, Record<string, any>>,
  res: Response,
  error: components["schemas"]["Problem"],
): Promise<Response> => {
  return res.status(error.status).json(
    req.accepts("problem+json")
      ? error
      : {
          code: error.status,
          message: error.title,
        },
  );
};

export const errorWrapper = async <T extends keyof operations & string>(
  req: Request<ParamsDictionary, any, any, Record<string, any>>,
  res: Response,
  result: Promise<ServiceReturnType<T>>,
): Promise<ServiceReturnType<T>["data"] | undefined> => {
  try {
    const r = await result;
    if (r.error) {
      responseWithError(req, res, r.error);
      return undefined;
    }
    return r.data;
  } catch (error) {
    responseWithError(req, res, {
      status: 500,
      title: "Internal Server Error",
    });
    throw error;
  }
};

export const errorWrapperV2 = async <T extends keyof operations & string>(
  req: Request<ParamsDictionary, any, any, Record<string, any>>,
  res: Response,
  result: Promise<ServiceReturnTypeV2<T>>,
): Promise<
  Extract<ServiceReturnTypeV2<T>, { success: true }>["data"] | undefined
> => {
  try {
    const r = await result;
    if (!r.success) {
      responseWithError(req, res, r.error);
      return undefined;
    }
    return r.data;
  } catch (error) {
    responseWithError(req, res, {
      status: 500,
      title: "Internal Server Error",
    });
    throw error;
  }
};
