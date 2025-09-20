export interface UserInfoJwt {
  id: string;
  auth_time: number;
  groups: string[];
  sub: string;
  iss: string;
  exp: number;
  aud: string;
  ver: number;
  jti: string;
  iat: number;
  scp: string[];
  cid: string;
  uid: string;
}
