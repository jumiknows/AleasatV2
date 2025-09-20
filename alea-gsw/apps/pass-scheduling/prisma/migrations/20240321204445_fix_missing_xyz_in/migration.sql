/*
  Warnings:

  - Added the required column `riseX` to the `ActualRiseSet` table without a default value. This is not possible if the table is not empty.
  - Added the required column `riseY` to the `ActualRiseSet` table without a default value. This is not possible if the table is not empty.
  - Added the required column `riseZ` to the `ActualRiseSet` table without a default value. This is not possible if the table is not empty.
  - Added the required column `setX` to the `ActualRiseSet` table without a default value. This is not possible if the table is not empty.
  - Added the required column `setY` to the `ActualRiseSet` table without a default value. This is not possible if the table is not empty.
  - Added the required column `setZ` to the `ActualRiseSet` table without a default value. This is not possible if the table is not empty.

*/
-- AlterTable
ALTER TABLE "ActualRiseSet" ADD COLUMN     "riseX" DOUBLE PRECISION NOT NULL,
ADD COLUMN     "riseY" DOUBLE PRECISION NOT NULL,
ADD COLUMN     "riseZ" DOUBLE PRECISION NOT NULL,
ADD COLUMN     "setX" DOUBLE PRECISION NOT NULL,
ADD COLUMN     "setY" DOUBLE PRECISION NOT NULL,
ADD COLUMN     "setZ" DOUBLE PRECISION NOT NULL;
