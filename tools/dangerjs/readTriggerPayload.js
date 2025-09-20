import fs from 'fs';

const file = fs.readFileSync(process.env.TRIGGER_PAYLOAD, 'utf8');
const payload = JSON.parse(file);
console.log(payload.iid);
