-- CreateEnum
CREATE TYPE "JobType" AS ENUM ('GMAT');

-- CreateTable
CREATE TABLE "Job" (
    "id" UUID NOT NULL,
    "name" TEXT NOT NULL,
    "type" "JobType" NOT NULL,
    "status" TEXT NOT NULL,
    "requestData" JSONB NOT NULL,
    "resultData" JSONB,

    CONSTRAINT "Job_pkey" PRIMARY KEY ("id")
);

-- CreateIndex
CREATE UNIQUE INDEX "Job_id_key" ON "Job"("id");
