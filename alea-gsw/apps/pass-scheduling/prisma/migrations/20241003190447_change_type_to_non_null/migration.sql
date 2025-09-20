/*
  Warnings:

  - Made the column `rise` on table `ActualRiseSet` required. This step will fail if there are existing NULL values in that column.
  - Made the column `set` on table `ActualRiseSet` required. This step will fail if there are existing NULL values in that column.
  - Made the column `rise` on table `PredictedRiseSet` required. This step will fail if there are existing NULL values in that column.
  - Made the column `set` on table `PredictedRiseSet` required. This step will fail if there are existing NULL values in that column.

*/
-- AlterTable
ALTER TABLE "ActualRiseSet" ALTER COLUMN "rise" SET NOT NULL,
ALTER COLUMN "set" SET NOT NULL;

-- AlterTable
ALTER TABLE "PredictedRiseSet" ALTER COLUMN "rise" SET NOT NULL,
ALTER COLUMN "set" SET NOT NULL;
