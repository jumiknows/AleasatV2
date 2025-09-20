import axios from "axios";

export const fetcher = async (url: string, token: string) =>
  await axios
    .get(url, { headers: { Authorization: "Bearer " + token } })
    .then((res) => res.data);
