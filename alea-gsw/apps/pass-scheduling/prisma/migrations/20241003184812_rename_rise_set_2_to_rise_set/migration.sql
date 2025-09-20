/*
  Warnings:

  - You are about to drop the column `rise2` on the `ActualRiseSet` table. All the data in the column will be lost.
  - You are about to drop the column `set2` on the `ActualRiseSet` table. All the data in the column will be lost.
  - You are about to drop the column `rise2` on the `PredictedRiseSet` table. All the data in the column will be lost.
  - You are about to drop the column `set2` on the `PredictedRiseSet` table. All the data in the column will be lost.
  - Changed the type of `rise` on the `ActualRiseSet` table. No cast exists, the column would be dropped and recreated, which cannot be done if there is data, since the column is required.
  - Changed the type of `set` on the `ActualRiseSet` table. No cast exists, the column would be dropped and recreated, which cannot be done if there is data, since the column is required.
  - Changed the type of `rise` on the `PredictedRiseSet` table. No cast exists, the column would be dropped and recreated, which cannot be done if there is data, since the column is required.
  - Changed the type of `set` on the `PredictedRiseSet` table. No cast exists, the column would be dropped and recreated, which cannot be done if there is data, since the column is required.

*/
-- AlterTable
ALTER TABLE "ActualRiseSet"
DROP COLUMN "rise";

ALTER TABLE "ActualRiseSet"
DROP COLUMN "set";

ALTER TABLE "ActualRiseSet"
RENAME COLUMN "rise2" TO "rise";

ALTER TABLE "ActualRiseSet"
RENAME COLUMN "set2" TO "set";

-- AlterTable
ALTER TABLE "PredictedRiseSet"
DROP COLUMN "rise";

ALTER TABLE "PredictedRiseSet"
DROP COLUMN "set";

ALTER TABLE "PredictedRiseSet"
RENAME COLUMN "rise2" TO "rise";

ALTER TABLE "PredictedRiseSet"
RENAME COLUMN "set2" TO "set";

