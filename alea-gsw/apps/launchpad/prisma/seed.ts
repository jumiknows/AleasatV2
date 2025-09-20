import { PrismaClient } from '../src/repositories/prisma.ts';
import { v4 as uuidv4 } from 'uuid';
const prisma = new PrismaClient();

async function main() {

  const now = new Date()
  const twoHoursBefore = new Date(now.getTime() - 2 * 60 * 60 * 1000)
  const twoHoursAfter = new Date(now.getTime() + 2 * 60 * 60 * 1000)

  const inc = 5 * 60 * 1000
  let idx = 0
  for (let i = twoHoursBefore.getTime(); i <= twoHoursAfter.getTime(); i += inc) {
    const launchpad = await makeLaunchpad(idx);
    const userSession = await makeUserSession(idx, launchpad, i, i + 60 * 60 * 1000)
    idx++
  }

  async function makeUserSession(i: number, launchpad: 
    { id: string; name: string; node: string; 
      port: string; serviceName: string; 
      fwCommit: string; }, startTime: number, endTime: number) {
    return await prisma.userSession.create({
      data: {
        userId: `id${i}`,
        startAt: new Date(startTime),
        endAt: new Date(endTime),
        launchpadId: launchpad.id
      }
    });
  }

  async function makeLaunchpad(i: number) {
    return await prisma.launchpad.create({
      data: {
        id: uuidv4(),
        name: `string${i}`,
        node: `string${i}`,
        port: `string${i}`,
        serviceName: `string${i}`,
        fwCommit: `string${i}`
      }
    });
  }
}

main()
  .catch((e) => {
    console.error(e);
    process.exit(1);
  })
  .finally(async () => {
    await prisma.$disconnect();
  });
