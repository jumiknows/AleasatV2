-- AlterTable
ALTER TABLE "ActualRiseSet" ADD COLUMN     "rise2" TIMESTAMP(3),
ADD COLUMN     "set2" TIMESTAMP(3);

-- AlterTable
ALTER TABLE "PredictedRiseSet" ADD COLUMN     "rise2" TIMESTAMP(3),
ADD COLUMN     "set2" TIMESTAMP(3);
