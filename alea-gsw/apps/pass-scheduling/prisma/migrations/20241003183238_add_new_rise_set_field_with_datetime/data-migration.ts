import { prisma } from "../../../src/repositories/prisma.js";

const predictedRiseSet = await prisma.predictedRiseSet.findMany();
for (const [i, pass] of predictedRiseSet.entries()) {
  console.log(pass.id, `${i}/${predictedRiseSet.length}`);
  await prisma.predictedRiseSet.update({
    where: { id: pass.id },
    data: {
      rise2: new Date(pass.rise * 1000),
      set2: new Date(pass.set * 1000),
    },
  });
}

const actualRiseSet = await prisma.actualRiseSet.findMany();
for (const [i, pass] of actualRiseSet.entries()) {
  console.log(pass.id, `${i}/${predictedRiseSet.length}`);
  await prisma.actualRiseSet.update({
    where: { id: pass.id },
    data: {
      rise2: new Date(pass.rise * 1000),
      set2: new Date(pass.set * 1000),
    },
  });
}
