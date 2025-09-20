/*
  Warnings:

  - Made the column `missionId` on table `MissionSchedule` required. This step will fail if there are existing NULL values in that column.

*/
-- AlterEnum
ALTER TYPE "MissionStatus" ADD VALUE 'sent';

-- DropForeignKey
ALTER TABLE "MissionSchedule" DROP CONSTRAINT "MissionSchedule_missionId_fkey";

-- AlterTable
ALTER TABLE "MissionSchedule" ALTER COLUMN "missionId" SET NOT NULL;

-- AddForeignKey
ALTER TABLE "MissionSchedule" ADD CONSTRAINT "MissionSchedule_missionId_fkey" FOREIGN KEY ("missionId") REFERENCES "Mission"("id") ON DELETE RESTRICT ON UPDATE CASCADE;
