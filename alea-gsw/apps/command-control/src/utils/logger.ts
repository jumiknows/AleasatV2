import pino from "pino";

const transport = pino.transport({
  targets: [
    {
      target: "pino-pretty",
      level:
        process.env.DEBUG === "true"
          ? "debug"
          : process.env.LOG_LEVEL || "info",
      options: {
        include: process.env.DEBUG === "true" ? undefined : "time,level,msg",
      },
    },
  ],
});

const logger = pino(transport);

export default logger;
