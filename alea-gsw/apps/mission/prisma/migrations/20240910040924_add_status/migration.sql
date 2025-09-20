-- CreateEnum
CREATE TYPE "MissionStatus" AS ENUM ('created', 'scheduled', 'sent', 'error');

-- AlterTable
ALTER TABLE "Mission" ADD COLUMN     "status" "MissionStatus" NOT NULL DEFAULT 'created';
