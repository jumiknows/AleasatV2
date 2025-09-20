import { type GraphQLClient } from "graphql-request";
import { type GraphQLClientRequestHeaders } from "graphql-request/build/esm/types.js";
import gql from "graphql-tag";
export type Maybe<T> = T | null;
export type InputMaybe<T> = Maybe<T>;
export type Exact<T extends Record<string, unknown>> = {
  [K in keyof T]: T[K];
};
export type MakeOptional<T, K extends keyof T> = Omit<T, K> & {
  [SubKey in K]?: Maybe<T[SubKey]>;
};
export type MakeMaybe<T, K extends keyof T> = Omit<T, K> & {
  [SubKey in K]: Maybe<T[SubKey]>;
};
export type MakeEmpty<T extends Record<string, unknown>, K extends keyof T> = {
  [_ in K]?: never;
};
export type Incremental<T> =
  | T
  | {
      [P in keyof T]?: P extends " $fragmentName" | "__typename" ? T[P] : never;
    };
/** All built-in and custom scalars, mapped to their actual values */
export interface Scalars {
  ID: { input: string; output: string };
  String: { input: string; output: string };
  Boolean: { input: boolean; output: boolean };
  Int: { input: number; output: number };
  Float: { input: number; output: number };
  BigInt: { input: any; output: any };
  DateTime: { input: any; output: any };
  Json: { input: any; output: any };
}

export interface ActualRiseSet {
  __typename?: "ActualRiseSet";
  Pass: Pass;
  id: Scalars["ID"]["output"];
  passId: Scalars["String"]["output"];
  rise: Scalars["Int"]["output"];
  set: Scalars["Int"]["output"];
}

export interface ActualRiseSetAvgOrderByAggregateInput {
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export interface ActualRiseSetCountOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export interface ActualRiseSetCreateInput {
  Pass: PassCreateNestedOneWithoutActualRiseSetInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
}

export interface ActualRiseSetCreateManyInput {
  id?: InputMaybe<Scalars["String"]["input"]>;
  passId: Scalars["String"]["input"];
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
}

export interface ActualRiseSetCreateNestedOneWithoutPassInput {
  connect?: InputMaybe<ActualRiseSetWhereUniqueInput>;
  connectOrCreate?: InputMaybe<ActualRiseSetCreateOrConnectWithoutPassInput>;
  create?: InputMaybe<ActualRiseSetCreateWithoutPassInput>;
}

export interface ActualRiseSetCreateOrConnectWithoutPassInput {
  create: ActualRiseSetCreateWithoutPassInput;
  where: ActualRiseSetWhereUniqueInput;
}

export interface ActualRiseSetCreateWithoutPassInput {
  id?: InputMaybe<Scalars["String"]["input"]>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
}

export interface ActualRiseSetMaxOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export interface ActualRiseSetMinOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export interface ActualRiseSetNullableRelationFilter {
  is?: InputMaybe<ActualRiseSetWhereInput>;
  isNot?: InputMaybe<ActualRiseSetWhereInput>;
}

export interface ActualRiseSetOrderByWithAggregationInput {
  _avg?: InputMaybe<ActualRiseSetAvgOrderByAggregateInput>;
  _count?: InputMaybe<ActualRiseSetCountOrderByAggregateInput>;
  _max?: InputMaybe<ActualRiseSetMaxOrderByAggregateInput>;
  _min?: InputMaybe<ActualRiseSetMinOrderByAggregateInput>;
  _sum?: InputMaybe<ActualRiseSetSumOrderByAggregateInput>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export interface ActualRiseSetOrderByWithRelationInput {
  Pass?: InputMaybe<PassOrderByWithRelationInput>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export enum ActualRiseSetScalarFieldEnum {
  Id = "id",
  PassId = "passId",
  Rise = "rise",
  Set = "set",
}

export interface ActualRiseSetScalarWhereWithAggregatesInput {
  AND?: InputMaybe<ActualRiseSetScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<ActualRiseSetScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<ActualRiseSetScalarWhereWithAggregatesInput[]>;
  id?: InputMaybe<UuidWithAggregatesFilter>;
  passId?: InputMaybe<UuidWithAggregatesFilter>;
  rise?: InputMaybe<IntWithAggregatesFilter>;
  set?: InputMaybe<IntWithAggregatesFilter>;
}

export interface ActualRiseSetSumOrderByAggregateInput {
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export interface ActualRiseSetUpdateInput {
  Pass?: InputMaybe<PassUpdateOneRequiredWithoutActualRiseSetNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  rise?: InputMaybe<IntFieldUpdateOperationsInput>;
  set?: InputMaybe<IntFieldUpdateOperationsInput>;
}

export interface ActualRiseSetUpdateManyMutationInput {
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  rise?: InputMaybe<IntFieldUpdateOperationsInput>;
  set?: InputMaybe<IntFieldUpdateOperationsInput>;
}

export interface ActualRiseSetUpdateOneWithoutPassNestedInput {
  connect?: InputMaybe<ActualRiseSetWhereUniqueInput>;
  connectOrCreate?: InputMaybe<ActualRiseSetCreateOrConnectWithoutPassInput>;
  create?: InputMaybe<ActualRiseSetCreateWithoutPassInput>;
  delete?: InputMaybe<ActualRiseSetWhereInput>;
  disconnect?: InputMaybe<ActualRiseSetWhereInput>;
  update?: InputMaybe<ActualRiseSetUpdateToOneWithWhereWithoutPassInput>;
  upsert?: InputMaybe<ActualRiseSetUpsertWithoutPassInput>;
}

export interface ActualRiseSetUpdateToOneWithWhereWithoutPassInput {
  data: ActualRiseSetUpdateWithoutPassInput;
  where?: InputMaybe<ActualRiseSetWhereInput>;
}

export interface ActualRiseSetUpdateWithoutPassInput {
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  rise?: InputMaybe<IntFieldUpdateOperationsInput>;
  set?: InputMaybe<IntFieldUpdateOperationsInput>;
}

export interface ActualRiseSetUpsertWithoutPassInput {
  create: ActualRiseSetCreateWithoutPassInput;
  update: ActualRiseSetUpdateWithoutPassInput;
  where?: InputMaybe<ActualRiseSetWhereInput>;
}

export interface ActualRiseSetWhereInput {
  AND?: InputMaybe<ActualRiseSetWhereInput[]>;
  NOT?: InputMaybe<ActualRiseSetWhereInput[]>;
  OR?: InputMaybe<ActualRiseSetWhereInput[]>;
  Pass?: InputMaybe<PassWhereInput>;
  id?: InputMaybe<UuidFilter>;
  passId?: InputMaybe<UuidFilter>;
  rise?: InputMaybe<IntFilter>;
  set?: InputMaybe<IntFilter>;
}

export interface ActualRiseSetWhereUniqueInput {
  AND?: InputMaybe<ActualRiseSetWhereInput[]>;
  NOT?: InputMaybe<ActualRiseSetWhereInput[]>;
  OR?: InputMaybe<ActualRiseSetWhereInput[]>;
  Pass?: InputMaybe<PassWhereInput>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  passId?: InputMaybe<Scalars["String"]["input"]>;
  rise?: InputMaybe<IntFilter>;
  set?: InputMaybe<IntFilter>;
}

export interface Admin {
  __typename?: "Admin";
  User: User;
  id: Scalars["ID"]["output"];
  reviewedCommand: Command[];
  userId: Scalars["Int"]["output"];
}

export interface AdminReviewedCommandArgs {
  cursor?: InputMaybe<CommandWhereUniqueInput>;
  distinct?: InputMaybe<CommandScalarFieldEnum[]>;
  orderBy?: InputMaybe<CommandOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<CommandWhereInput>;
}

export interface AdminAvgOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface AdminCountOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface AdminCreateInput {
  User: UserCreateNestedOneWithoutAdminInput;
  id: Scalars["Int"]["input"];
  reviewedCommand?: InputMaybe<CommandCreateNestedManyWithoutReviewerInput>;
}

export interface AdminCreateManyInput {
  id: Scalars["Int"]["input"];
  userId: Scalars["Int"]["input"];
}

export interface AdminCreateNestedOneWithoutReviewedCommandInput {
  connect?: InputMaybe<AdminWhereUniqueInput>;
  connectOrCreate?: InputMaybe<AdminCreateOrConnectWithoutReviewedCommandInput>;
  create?: InputMaybe<AdminCreateWithoutReviewedCommandInput>;
}

export interface AdminCreateNestedOneWithoutUserInput {
  connect?: InputMaybe<AdminWhereUniqueInput>;
  connectOrCreate?: InputMaybe<AdminCreateOrConnectWithoutUserInput>;
  create?: InputMaybe<AdminCreateWithoutUserInput>;
}

export interface AdminCreateOrConnectWithoutReviewedCommandInput {
  create: AdminCreateWithoutReviewedCommandInput;
  where: AdminWhereUniqueInput;
}

export interface AdminCreateOrConnectWithoutUserInput {
  create: AdminCreateWithoutUserInput;
  where: AdminWhereUniqueInput;
}

export interface AdminCreateWithoutReviewedCommandInput {
  User: UserCreateNestedOneWithoutAdminInput;
  id: Scalars["Int"]["input"];
}

export interface AdminCreateWithoutUserInput {
  id: Scalars["Int"]["input"];
  reviewedCommand?: InputMaybe<CommandCreateNestedManyWithoutReviewerInput>;
}

export interface AdminMaxOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface AdminMinOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface AdminNullableRelationFilter {
  is?: InputMaybe<AdminWhereInput>;
  isNot?: InputMaybe<AdminWhereInput>;
}

export interface AdminOrderByWithAggregationInput {
  _avg?: InputMaybe<AdminAvgOrderByAggregateInput>;
  _count?: InputMaybe<AdminCountOrderByAggregateInput>;
  _max?: InputMaybe<AdminMaxOrderByAggregateInput>;
  _min?: InputMaybe<AdminMinOrderByAggregateInput>;
  _sum?: InputMaybe<AdminSumOrderByAggregateInput>;
  id?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface AdminOrderByWithRelationInput {
  User?: InputMaybe<UserOrderByWithRelationInput>;
  id?: InputMaybe<SortOrder>;
  reviewedCommand?: InputMaybe<CommandOrderByRelationAggregateInput>;
  userId?: InputMaybe<SortOrder>;
}

export enum AdminScalarFieldEnum {
  Id = "id",
  UserId = "userId",
}

export interface AdminScalarWhereWithAggregatesInput {
  AND?: InputMaybe<AdminScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<AdminScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<AdminScalarWhereWithAggregatesInput[]>;
  id?: InputMaybe<IntWithAggregatesFilter>;
  userId?: InputMaybe<IntWithAggregatesFilter>;
}

export interface AdminSumOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface AdminUpdateInput {
  User?: InputMaybe<UserUpdateOneRequiredWithoutAdminNestedInput>;
  id?: InputMaybe<IntFieldUpdateOperationsInput>;
  reviewedCommand?: InputMaybe<CommandUpdateManyWithoutReviewerNestedInput>;
}

export interface AdminUpdateManyMutationInput {
  id?: InputMaybe<IntFieldUpdateOperationsInput>;
}

export interface AdminUpdateOneWithoutReviewedCommandNestedInput {
  connect?: InputMaybe<AdminWhereUniqueInput>;
  connectOrCreate?: InputMaybe<AdminCreateOrConnectWithoutReviewedCommandInput>;
  create?: InputMaybe<AdminCreateWithoutReviewedCommandInput>;
  delete?: InputMaybe<AdminWhereInput>;
  disconnect?: InputMaybe<AdminWhereInput>;
  update?: InputMaybe<AdminUpdateToOneWithWhereWithoutReviewedCommandInput>;
  upsert?: InputMaybe<AdminUpsertWithoutReviewedCommandInput>;
}

export interface AdminUpdateOneWithoutUserNestedInput {
  connect?: InputMaybe<AdminWhereUniqueInput>;
  connectOrCreate?: InputMaybe<AdminCreateOrConnectWithoutUserInput>;
  create?: InputMaybe<AdminCreateWithoutUserInput>;
  delete?: InputMaybe<AdminWhereInput>;
  disconnect?: InputMaybe<AdminWhereInput>;
  update?: InputMaybe<AdminUpdateToOneWithWhereWithoutUserInput>;
  upsert?: InputMaybe<AdminUpsertWithoutUserInput>;
}

export interface AdminUpdateToOneWithWhereWithoutReviewedCommandInput {
  data: AdminUpdateWithoutReviewedCommandInput;
  where?: InputMaybe<AdminWhereInput>;
}

export interface AdminUpdateToOneWithWhereWithoutUserInput {
  data: AdminUpdateWithoutUserInput;
  where?: InputMaybe<AdminWhereInput>;
}

export interface AdminUpdateWithoutReviewedCommandInput {
  User?: InputMaybe<UserUpdateOneRequiredWithoutAdminNestedInput>;
  id?: InputMaybe<IntFieldUpdateOperationsInput>;
}

export interface AdminUpdateWithoutUserInput {
  id?: InputMaybe<IntFieldUpdateOperationsInput>;
  reviewedCommand?: InputMaybe<CommandUpdateManyWithoutReviewerNestedInput>;
}

export interface AdminUpsertWithoutReviewedCommandInput {
  create: AdminCreateWithoutReviewedCommandInput;
  update: AdminUpdateWithoutReviewedCommandInput;
  where?: InputMaybe<AdminWhereInput>;
}

export interface AdminUpsertWithoutUserInput {
  create: AdminCreateWithoutUserInput;
  update: AdminUpdateWithoutUserInput;
  where?: InputMaybe<AdminWhereInput>;
}

export interface AdminWhereInput {
  AND?: InputMaybe<AdminWhereInput[]>;
  NOT?: InputMaybe<AdminWhereInput[]>;
  OR?: InputMaybe<AdminWhereInput[]>;
  User?: InputMaybe<UserWhereInput>;
  id?: InputMaybe<IntFilter>;
  reviewedCommand?: InputMaybe<CommandListRelationFilter>;
  userId?: InputMaybe<IntFilter>;
}

export interface AdminWhereUniqueInput {
  AND?: InputMaybe<AdminWhereInput[]>;
  NOT?: InputMaybe<AdminWhereInput[]>;
  OR?: InputMaybe<AdminWhereInput[]>;
  User?: InputMaybe<UserWhereInput>;
  id?: InputMaybe<Scalars["Int"]["input"]>;
  reviewedCommand?: InputMaybe<CommandListRelationFilter>;
  userId?: InputMaybe<Scalars["Int"]["input"]>;
}

/** Batch payloads from prisma. */
export interface BatchPayload {
  __typename?: "BatchPayload";
  /** Prisma Batch Payload */
  count: Scalars["Int"]["output"];
}

export interface BigIntFieldUpdateOperationsInput {
  decrement?: InputMaybe<Scalars["BigInt"]["input"]>;
  divide?: InputMaybe<Scalars["BigInt"]["input"]>;
  increment?: InputMaybe<Scalars["BigInt"]["input"]>;
  multiply?: InputMaybe<Scalars["BigInt"]["input"]>;
  set?: InputMaybe<Scalars["BigInt"]["input"]>;
}

export interface BigIntFilter {
  equals?: InputMaybe<Scalars["BigInt"]["input"]>;
  gt?: InputMaybe<Scalars["BigInt"]["input"]>;
  gte?: InputMaybe<Scalars["BigInt"]["input"]>;
  in?: InputMaybe<Array<Scalars["BigInt"]["input"]>>;
  lt?: InputMaybe<Scalars["BigInt"]["input"]>;
  lte?: InputMaybe<Scalars["BigInt"]["input"]>;
  not?: InputMaybe<NestedBigIntFilter>;
  notIn?: InputMaybe<Array<Scalars["BigInt"]["input"]>>;
}

export interface BigIntWithAggregatesFilter {
  _avg?: InputMaybe<NestedFloatFilter>;
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedBigIntFilter>;
  _min?: InputMaybe<NestedBigIntFilter>;
  _sum?: InputMaybe<NestedBigIntFilter>;
  equals?: InputMaybe<Scalars["BigInt"]["input"]>;
  gt?: InputMaybe<Scalars["BigInt"]["input"]>;
  gte?: InputMaybe<Scalars["BigInt"]["input"]>;
  in?: InputMaybe<Array<Scalars["BigInt"]["input"]>>;
  lt?: InputMaybe<Scalars["BigInt"]["input"]>;
  lte?: InputMaybe<Scalars["BigInt"]["input"]>;
  not?: InputMaybe<NestedBigIntWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["BigInt"]["input"]>>;
}

export interface BoolFieldUpdateOperationsInput {
  set?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface BoolFilter {
  equals?: InputMaybe<Scalars["Boolean"]["input"]>;
  not?: InputMaybe<NestedBoolFilter>;
}

export interface BoolNullableFilter {
  equals?: InputMaybe<Scalars["Boolean"]["input"]>;
  not?: InputMaybe<NestedBoolNullableFilter>;
}

export interface BoolNullableWithAggregatesFilter {
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedBoolNullableFilter>;
  _min?: InputMaybe<NestedBoolNullableFilter>;
  equals?: InputMaybe<Scalars["Boolean"]["input"]>;
  not?: InputMaybe<NestedBoolNullableWithAggregatesFilter>;
}

export interface BoolWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedBoolFilter>;
  _min?: InputMaybe<NestedBoolFilter>;
  equals?: InputMaybe<Scalars["Boolean"]["input"]>;
  not?: InputMaybe<NestedBoolWithAggregatesFilter>;
}

export interface Command {
  __typename?: "Command";
  Pass?: Maybe<Pass>;
  command: Scalars["String"]["output"];
  id: Scalars["ID"]["output"];
  passId?: Maybe<Scalars["String"]["output"]>;
  power: Scalars["Float"]["output"];
  result?: Maybe<Result>;
  reviewComment?: Maybe<Scalars["String"]["output"]>;
  reviewPassed?: Maybe<Scalars["Boolean"]["output"]>;
  reviewer?: Maybe<Admin>;
  reviewerId?: Maybe<Scalars["Int"]["output"]>;
  sentAt?: Maybe<Scalars["DateTime"]["output"]>;
  size: Scalars["Int"]["output"];
  submitedAt: Scalars["DateTime"]["output"];
  transfer: Transfer[];
  user: User;
  userId: Scalars["Int"]["output"];
}

export interface CommandTransferArgs {
  cursor?: InputMaybe<TransferWhereUniqueInput>;
  distinct?: InputMaybe<TransferScalarFieldEnum[]>;
  orderBy?: InputMaybe<TransferOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<TransferWhereInput>;
}

export interface CommandAvgOrderByAggregateInput {
  power?: InputMaybe<SortOrder>;
  reviewerId?: InputMaybe<SortOrder>;
  size?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface CommandCountOrderByAggregateInput {
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  power?: InputMaybe<SortOrder>;
  reviewComment?: InputMaybe<SortOrder>;
  reviewPassed?: InputMaybe<SortOrder>;
  reviewerId?: InputMaybe<SortOrder>;
  sentAt?: InputMaybe<SortOrder>;
  size?: InputMaybe<SortOrder>;
  submitedAt?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface CommandCreateInput {
  Pass?: InputMaybe<PassCreateNestedOneWithoutCommandInput>;
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  result?: InputMaybe<ResultCreateNestedOneWithoutCommandInput>;
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  reviewer?: InputMaybe<AdminCreateNestedOneWithoutReviewedCommandInput>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutCommandInput>;
  user: UserCreateNestedOneWithoutRequestedCommandInput;
}

export interface CommandCreateManyInput {
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  passId?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  reviewerId?: InputMaybe<Scalars["Int"]["input"]>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  userId: Scalars["Int"]["input"];
}

export interface CommandCreateManyPassInput {
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  reviewerId?: InputMaybe<Scalars["Int"]["input"]>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  userId: Scalars["Int"]["input"];
}

export interface CommandCreateManyPassInputEnvelope {
  data: CommandCreateManyPassInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface CommandCreateManyReviewerInput {
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  passId?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  userId: Scalars["Int"]["input"];
}

export interface CommandCreateManyReviewerInputEnvelope {
  data: CommandCreateManyReviewerInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface CommandCreateManyUserInput {
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  passId?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  reviewerId?: InputMaybe<Scalars["Int"]["input"]>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
}

export interface CommandCreateManyUserInputEnvelope {
  data: CommandCreateManyUserInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface CommandCreateNestedManyWithoutPassInput {
  connect?: InputMaybe<CommandWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutPassInput[]>;
  create?: InputMaybe<CommandCreateWithoutPassInput[]>;
  createMany?: InputMaybe<CommandCreateManyPassInputEnvelope>;
}

export interface CommandCreateNestedManyWithoutReviewerInput {
  connect?: InputMaybe<CommandWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutReviewerInput[]>;
  create?: InputMaybe<CommandCreateWithoutReviewerInput[]>;
  createMany?: InputMaybe<CommandCreateManyReviewerInputEnvelope>;
}

export interface CommandCreateNestedManyWithoutUserInput {
  connect?: InputMaybe<CommandWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutUserInput[]>;
  create?: InputMaybe<CommandCreateWithoutUserInput[]>;
  createMany?: InputMaybe<CommandCreateManyUserInputEnvelope>;
}

export interface CommandCreateNestedOneWithoutResultInput {
  connect?: InputMaybe<CommandWhereUniqueInput>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutResultInput>;
  create?: InputMaybe<CommandCreateWithoutResultInput>;
}

export interface CommandCreateNestedOneWithoutTransferInput {
  connect?: InputMaybe<CommandWhereUniqueInput>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutTransferInput>;
  create?: InputMaybe<CommandCreateWithoutTransferInput>;
}

export interface CommandCreateOrConnectWithoutPassInput {
  create: CommandCreateWithoutPassInput;
  where: CommandWhereUniqueInput;
}

export interface CommandCreateOrConnectWithoutResultInput {
  create: CommandCreateWithoutResultInput;
  where: CommandWhereUniqueInput;
}

export interface CommandCreateOrConnectWithoutReviewerInput {
  create: CommandCreateWithoutReviewerInput;
  where: CommandWhereUniqueInput;
}

export interface CommandCreateOrConnectWithoutTransferInput {
  create: CommandCreateWithoutTransferInput;
  where: CommandWhereUniqueInput;
}

export interface CommandCreateOrConnectWithoutUserInput {
  create: CommandCreateWithoutUserInput;
  where: CommandWhereUniqueInput;
}

export interface CommandCreateWithoutPassInput {
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  result?: InputMaybe<ResultCreateNestedOneWithoutCommandInput>;
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  reviewer?: InputMaybe<AdminCreateNestedOneWithoutReviewedCommandInput>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutCommandInput>;
  user: UserCreateNestedOneWithoutRequestedCommandInput;
}

export interface CommandCreateWithoutResultInput {
  Pass?: InputMaybe<PassCreateNestedOneWithoutCommandInput>;
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  reviewer?: InputMaybe<AdminCreateNestedOneWithoutReviewedCommandInput>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutCommandInput>;
  user: UserCreateNestedOneWithoutRequestedCommandInput;
}

export interface CommandCreateWithoutReviewerInput {
  Pass?: InputMaybe<PassCreateNestedOneWithoutCommandInput>;
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  result?: InputMaybe<ResultCreateNestedOneWithoutCommandInput>;
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutCommandInput>;
  user: UserCreateNestedOneWithoutRequestedCommandInput;
}

export interface CommandCreateWithoutTransferInput {
  Pass?: InputMaybe<PassCreateNestedOneWithoutCommandInput>;
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  result?: InputMaybe<ResultCreateNestedOneWithoutCommandInput>;
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  reviewer?: InputMaybe<AdminCreateNestedOneWithoutReviewedCommandInput>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  user: UserCreateNestedOneWithoutRequestedCommandInput;
}

export interface CommandCreateWithoutUserInput {
  Pass?: InputMaybe<PassCreateNestedOneWithoutCommandInput>;
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  power: Scalars["Float"]["input"];
  result?: InputMaybe<ResultCreateNestedOneWithoutCommandInput>;
  reviewComment?: InputMaybe<Scalars["String"]["input"]>;
  reviewPassed?: InputMaybe<Scalars["Boolean"]["input"]>;
  reviewer?: InputMaybe<AdminCreateNestedOneWithoutReviewedCommandInput>;
  sentAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  size: Scalars["Int"]["input"];
  submitedAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutCommandInput>;
}

export interface CommandListRelationFilter {
  every?: InputMaybe<CommandWhereInput>;
  none?: InputMaybe<CommandWhereInput>;
  some?: InputMaybe<CommandWhereInput>;
}

export interface CommandMaxOrderByAggregateInput {
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  power?: InputMaybe<SortOrder>;
  reviewComment?: InputMaybe<SortOrder>;
  reviewPassed?: InputMaybe<SortOrder>;
  reviewerId?: InputMaybe<SortOrder>;
  sentAt?: InputMaybe<SortOrder>;
  size?: InputMaybe<SortOrder>;
  submitedAt?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface CommandMinOrderByAggregateInput {
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  power?: InputMaybe<SortOrder>;
  reviewComment?: InputMaybe<SortOrder>;
  reviewPassed?: InputMaybe<SortOrder>;
  reviewerId?: InputMaybe<SortOrder>;
  sentAt?: InputMaybe<SortOrder>;
  size?: InputMaybe<SortOrder>;
  submitedAt?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface CommandOrderByRelationAggregateInput {
  _count?: InputMaybe<SortOrder>;
}

export interface CommandOrderByWithAggregationInput {
  _avg?: InputMaybe<CommandAvgOrderByAggregateInput>;
  _count?: InputMaybe<CommandCountOrderByAggregateInput>;
  _max?: InputMaybe<CommandMaxOrderByAggregateInput>;
  _min?: InputMaybe<CommandMinOrderByAggregateInput>;
  _sum?: InputMaybe<CommandSumOrderByAggregateInput>;
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  power?: InputMaybe<SortOrder>;
  reviewComment?: InputMaybe<SortOrder>;
  reviewPassed?: InputMaybe<SortOrder>;
  reviewerId?: InputMaybe<SortOrder>;
  sentAt?: InputMaybe<SortOrder>;
  size?: InputMaybe<SortOrder>;
  submitedAt?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface CommandOrderByWithRelationInput {
  Pass?: InputMaybe<PassOrderByWithRelationInput>;
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  power?: InputMaybe<SortOrder>;
  result?: InputMaybe<ResultOrderByWithRelationInput>;
  reviewComment?: InputMaybe<SortOrder>;
  reviewPassed?: InputMaybe<SortOrder>;
  reviewer?: InputMaybe<AdminOrderByWithRelationInput>;
  reviewerId?: InputMaybe<SortOrder>;
  sentAt?: InputMaybe<SortOrder>;
  size?: InputMaybe<SortOrder>;
  submitedAt?: InputMaybe<SortOrder>;
  transfer?: InputMaybe<TransferOrderByRelationAggregateInput>;
  user?: InputMaybe<UserOrderByWithRelationInput>;
  userId?: InputMaybe<SortOrder>;
}

export interface CommandRelationFilter {
  is?: InputMaybe<CommandWhereInput>;
  isNot?: InputMaybe<CommandWhereInput>;
}

export enum CommandScalarFieldEnum {
  Command = "command",
  Id = "id",
  PassId = "passId",
  Power = "power",
  ReviewComment = "reviewComment",
  ReviewPassed = "reviewPassed",
  ReviewerId = "reviewerId",
  SentAt = "sentAt",
  Size = "size",
  SubmitedAt = "submitedAt",
  UserId = "userId",
}

export interface CommandScalarWhereInput {
  AND?: InputMaybe<CommandScalarWhereInput[]>;
  NOT?: InputMaybe<CommandScalarWhereInput[]>;
  OR?: InputMaybe<CommandScalarWhereInput[]>;
  command?: InputMaybe<StringFilter>;
  id?: InputMaybe<UuidFilter>;
  passId?: InputMaybe<UuidNullableFilter>;
  power?: InputMaybe<FloatFilter>;
  reviewComment?: InputMaybe<StringNullableFilter>;
  reviewPassed?: InputMaybe<BoolNullableFilter>;
  reviewerId?: InputMaybe<IntNullableFilter>;
  sentAt?: InputMaybe<DateTimeNullableFilter>;
  size?: InputMaybe<IntFilter>;
  submitedAt?: InputMaybe<DateTimeFilter>;
  userId?: InputMaybe<IntFilter>;
}

export interface CommandScalarWhereWithAggregatesInput {
  AND?: InputMaybe<CommandScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<CommandScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<CommandScalarWhereWithAggregatesInput[]>;
  command?: InputMaybe<StringWithAggregatesFilter>;
  id?: InputMaybe<UuidWithAggregatesFilter>;
  passId?: InputMaybe<UuidNullableWithAggregatesFilter>;
  power?: InputMaybe<FloatWithAggregatesFilter>;
  reviewComment?: InputMaybe<StringNullableWithAggregatesFilter>;
  reviewPassed?: InputMaybe<BoolNullableWithAggregatesFilter>;
  reviewerId?: InputMaybe<IntNullableWithAggregatesFilter>;
  sentAt?: InputMaybe<DateTimeNullableWithAggregatesFilter>;
  size?: InputMaybe<IntWithAggregatesFilter>;
  submitedAt?: InputMaybe<DateTimeWithAggregatesFilter>;
  userId?: InputMaybe<IntWithAggregatesFilter>;
}

export interface CommandSumOrderByAggregateInput {
  power?: InputMaybe<SortOrder>;
  reviewerId?: InputMaybe<SortOrder>;
  size?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface CommandUpdateInput {
  Pass?: InputMaybe<PassUpdateOneWithoutCommandNestedInput>;
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  power?: InputMaybe<FloatFieldUpdateOperationsInput>;
  result?: InputMaybe<ResultUpdateOneWithoutCommandNestedInput>;
  reviewComment?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
  reviewPassed?: InputMaybe<NullableBoolFieldUpdateOperationsInput>;
  reviewer?: InputMaybe<AdminUpdateOneWithoutReviewedCommandNestedInput>;
  sentAt?: InputMaybe<NullableDateTimeFieldUpdateOperationsInput>;
  size?: InputMaybe<IntFieldUpdateOperationsInput>;
  submitedAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutCommandNestedInput>;
  user?: InputMaybe<UserUpdateOneRequiredWithoutRequestedCommandNestedInput>;
}

export interface CommandUpdateManyMutationInput {
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  power?: InputMaybe<FloatFieldUpdateOperationsInput>;
  reviewComment?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
  reviewPassed?: InputMaybe<NullableBoolFieldUpdateOperationsInput>;
  sentAt?: InputMaybe<NullableDateTimeFieldUpdateOperationsInput>;
  size?: InputMaybe<IntFieldUpdateOperationsInput>;
  submitedAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
}

export interface CommandUpdateManyWithWhereWithoutPassInput {
  data: CommandUpdateManyMutationInput;
  where: CommandScalarWhereInput;
}

export interface CommandUpdateManyWithWhereWithoutReviewerInput {
  data: CommandUpdateManyMutationInput;
  where: CommandScalarWhereInput;
}

export interface CommandUpdateManyWithWhereWithoutUserInput {
  data: CommandUpdateManyMutationInput;
  where: CommandScalarWhereInput;
}

export interface CommandUpdateManyWithoutPassNestedInput {
  connect?: InputMaybe<CommandWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutPassInput[]>;
  create?: InputMaybe<CommandCreateWithoutPassInput[]>;
  createMany?: InputMaybe<CommandCreateManyPassInputEnvelope>;
  delete?: InputMaybe<CommandWhereUniqueInput[]>;
  deleteMany?: InputMaybe<CommandScalarWhereInput[]>;
  disconnect?: InputMaybe<CommandWhereUniqueInput[]>;
  set?: InputMaybe<CommandWhereUniqueInput[]>;
  update?: InputMaybe<CommandUpdateWithWhereUniqueWithoutPassInput[]>;
  updateMany?: InputMaybe<CommandUpdateManyWithWhereWithoutPassInput[]>;
  upsert?: InputMaybe<CommandUpsertWithWhereUniqueWithoutPassInput[]>;
}

export interface CommandUpdateManyWithoutReviewerNestedInput {
  connect?: InputMaybe<CommandWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutReviewerInput[]>;
  create?: InputMaybe<CommandCreateWithoutReviewerInput[]>;
  createMany?: InputMaybe<CommandCreateManyReviewerInputEnvelope>;
  delete?: InputMaybe<CommandWhereUniqueInput[]>;
  deleteMany?: InputMaybe<CommandScalarWhereInput[]>;
  disconnect?: InputMaybe<CommandWhereUniqueInput[]>;
  set?: InputMaybe<CommandWhereUniqueInput[]>;
  update?: InputMaybe<CommandUpdateWithWhereUniqueWithoutReviewerInput[]>;
  updateMany?: InputMaybe<CommandUpdateManyWithWhereWithoutReviewerInput[]>;
  upsert?: InputMaybe<CommandUpsertWithWhereUniqueWithoutReviewerInput[]>;
}

export interface CommandUpdateManyWithoutUserNestedInput {
  connect?: InputMaybe<CommandWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutUserInput[]>;
  create?: InputMaybe<CommandCreateWithoutUserInput[]>;
  createMany?: InputMaybe<CommandCreateManyUserInputEnvelope>;
  delete?: InputMaybe<CommandWhereUniqueInput[]>;
  deleteMany?: InputMaybe<CommandScalarWhereInput[]>;
  disconnect?: InputMaybe<CommandWhereUniqueInput[]>;
  set?: InputMaybe<CommandWhereUniqueInput[]>;
  update?: InputMaybe<CommandUpdateWithWhereUniqueWithoutUserInput[]>;
  updateMany?: InputMaybe<CommandUpdateManyWithWhereWithoutUserInput[]>;
  upsert?: InputMaybe<CommandUpsertWithWhereUniqueWithoutUserInput[]>;
}

export interface CommandUpdateOneRequiredWithoutResultNestedInput {
  connect?: InputMaybe<CommandWhereUniqueInput>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutResultInput>;
  create?: InputMaybe<CommandCreateWithoutResultInput>;
  update?: InputMaybe<CommandUpdateToOneWithWhereWithoutResultInput>;
  upsert?: InputMaybe<CommandUpsertWithoutResultInput>;
}

export interface CommandUpdateOneRequiredWithoutTransferNestedInput {
  connect?: InputMaybe<CommandWhereUniqueInput>;
  connectOrCreate?: InputMaybe<CommandCreateOrConnectWithoutTransferInput>;
  create?: InputMaybe<CommandCreateWithoutTransferInput>;
  update?: InputMaybe<CommandUpdateToOneWithWhereWithoutTransferInput>;
  upsert?: InputMaybe<CommandUpsertWithoutTransferInput>;
}

export interface CommandUpdateToOneWithWhereWithoutResultInput {
  data: CommandUpdateWithoutResultInput;
  where?: InputMaybe<CommandWhereInput>;
}

export interface CommandUpdateToOneWithWhereWithoutTransferInput {
  data: CommandUpdateWithoutTransferInput;
  where?: InputMaybe<CommandWhereInput>;
}

export interface CommandUpdateWithWhereUniqueWithoutPassInput {
  data: CommandUpdateWithoutPassInput;
  where: CommandWhereUniqueInput;
}

export interface CommandUpdateWithWhereUniqueWithoutReviewerInput {
  data: CommandUpdateWithoutReviewerInput;
  where: CommandWhereUniqueInput;
}

export interface CommandUpdateWithWhereUniqueWithoutUserInput {
  data: CommandUpdateWithoutUserInput;
  where: CommandWhereUniqueInput;
}

export interface CommandUpdateWithoutPassInput {
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  power?: InputMaybe<FloatFieldUpdateOperationsInput>;
  result?: InputMaybe<ResultUpdateOneWithoutCommandNestedInput>;
  reviewComment?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
  reviewPassed?: InputMaybe<NullableBoolFieldUpdateOperationsInput>;
  reviewer?: InputMaybe<AdminUpdateOneWithoutReviewedCommandNestedInput>;
  sentAt?: InputMaybe<NullableDateTimeFieldUpdateOperationsInput>;
  size?: InputMaybe<IntFieldUpdateOperationsInput>;
  submitedAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutCommandNestedInput>;
  user?: InputMaybe<UserUpdateOneRequiredWithoutRequestedCommandNestedInput>;
}

export interface CommandUpdateWithoutResultInput {
  Pass?: InputMaybe<PassUpdateOneWithoutCommandNestedInput>;
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  power?: InputMaybe<FloatFieldUpdateOperationsInput>;
  reviewComment?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
  reviewPassed?: InputMaybe<NullableBoolFieldUpdateOperationsInput>;
  reviewer?: InputMaybe<AdminUpdateOneWithoutReviewedCommandNestedInput>;
  sentAt?: InputMaybe<NullableDateTimeFieldUpdateOperationsInput>;
  size?: InputMaybe<IntFieldUpdateOperationsInput>;
  submitedAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutCommandNestedInput>;
  user?: InputMaybe<UserUpdateOneRequiredWithoutRequestedCommandNestedInput>;
}

export interface CommandUpdateWithoutReviewerInput {
  Pass?: InputMaybe<PassUpdateOneWithoutCommandNestedInput>;
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  power?: InputMaybe<FloatFieldUpdateOperationsInput>;
  result?: InputMaybe<ResultUpdateOneWithoutCommandNestedInput>;
  reviewComment?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
  reviewPassed?: InputMaybe<NullableBoolFieldUpdateOperationsInput>;
  sentAt?: InputMaybe<NullableDateTimeFieldUpdateOperationsInput>;
  size?: InputMaybe<IntFieldUpdateOperationsInput>;
  submitedAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutCommandNestedInput>;
  user?: InputMaybe<UserUpdateOneRequiredWithoutRequestedCommandNestedInput>;
}

export interface CommandUpdateWithoutTransferInput {
  Pass?: InputMaybe<PassUpdateOneWithoutCommandNestedInput>;
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  power?: InputMaybe<FloatFieldUpdateOperationsInput>;
  result?: InputMaybe<ResultUpdateOneWithoutCommandNestedInput>;
  reviewComment?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
  reviewPassed?: InputMaybe<NullableBoolFieldUpdateOperationsInput>;
  reviewer?: InputMaybe<AdminUpdateOneWithoutReviewedCommandNestedInput>;
  sentAt?: InputMaybe<NullableDateTimeFieldUpdateOperationsInput>;
  size?: InputMaybe<IntFieldUpdateOperationsInput>;
  submitedAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
  user?: InputMaybe<UserUpdateOneRequiredWithoutRequestedCommandNestedInput>;
}

export interface CommandUpdateWithoutUserInput {
  Pass?: InputMaybe<PassUpdateOneWithoutCommandNestedInput>;
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  power?: InputMaybe<FloatFieldUpdateOperationsInput>;
  result?: InputMaybe<ResultUpdateOneWithoutCommandNestedInput>;
  reviewComment?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
  reviewPassed?: InputMaybe<NullableBoolFieldUpdateOperationsInput>;
  reviewer?: InputMaybe<AdminUpdateOneWithoutReviewedCommandNestedInput>;
  sentAt?: InputMaybe<NullableDateTimeFieldUpdateOperationsInput>;
  size?: InputMaybe<IntFieldUpdateOperationsInput>;
  submitedAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutCommandNestedInput>;
}

export interface CommandUpsertWithWhereUniqueWithoutPassInput {
  create: CommandCreateWithoutPassInput;
  update: CommandUpdateWithoutPassInput;
  where: CommandWhereUniqueInput;
}

export interface CommandUpsertWithWhereUniqueWithoutReviewerInput {
  create: CommandCreateWithoutReviewerInput;
  update: CommandUpdateWithoutReviewerInput;
  where: CommandWhereUniqueInput;
}

export interface CommandUpsertWithWhereUniqueWithoutUserInput {
  create: CommandCreateWithoutUserInput;
  update: CommandUpdateWithoutUserInput;
  where: CommandWhereUniqueInput;
}

export interface CommandUpsertWithoutResultInput {
  create: CommandCreateWithoutResultInput;
  update: CommandUpdateWithoutResultInput;
  where?: InputMaybe<CommandWhereInput>;
}

export interface CommandUpsertWithoutTransferInput {
  create: CommandCreateWithoutTransferInput;
  update: CommandUpdateWithoutTransferInput;
  where?: InputMaybe<CommandWhereInput>;
}

export interface CommandWhereInput {
  AND?: InputMaybe<CommandWhereInput[]>;
  NOT?: InputMaybe<CommandWhereInput[]>;
  OR?: InputMaybe<CommandWhereInput[]>;
  Pass?: InputMaybe<PassWhereInput>;
  command?: InputMaybe<StringFilter>;
  id?: InputMaybe<UuidFilter>;
  passId?: InputMaybe<UuidNullableFilter>;
  power?: InputMaybe<FloatFilter>;
  result?: InputMaybe<ResultWhereInput>;
  reviewComment?: InputMaybe<StringNullableFilter>;
  reviewPassed?: InputMaybe<BoolNullableFilter>;
  reviewer?: InputMaybe<AdminWhereInput>;
  reviewerId?: InputMaybe<IntNullableFilter>;
  sentAt?: InputMaybe<DateTimeNullableFilter>;
  size?: InputMaybe<IntFilter>;
  submitedAt?: InputMaybe<DateTimeFilter>;
  transfer?: InputMaybe<TransferListRelationFilter>;
  user?: InputMaybe<UserWhereInput>;
  userId?: InputMaybe<IntFilter>;
}

export interface CommandWhereUniqueInput {
  AND?: InputMaybe<CommandWhereInput[]>;
  NOT?: InputMaybe<CommandWhereInput[]>;
  OR?: InputMaybe<CommandWhereInput[]>;
  Pass?: InputMaybe<PassWhereInput>;
  command?: InputMaybe<StringFilter>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  passId?: InputMaybe<UuidNullableFilter>;
  power?: InputMaybe<FloatFilter>;
  result?: InputMaybe<ResultWhereInput>;
  reviewComment?: InputMaybe<StringNullableFilter>;
  reviewPassed?: InputMaybe<BoolNullableFilter>;
  reviewer?: InputMaybe<AdminWhereInput>;
  reviewerId?: InputMaybe<IntNullableFilter>;
  sentAt?: InputMaybe<DateTimeNullableFilter>;
  size?: InputMaybe<IntFilter>;
  submitedAt?: InputMaybe<DateTimeFilter>;
  transfer?: InputMaybe<TransferListRelationFilter>;
  user?: InputMaybe<UserWhereInput>;
  userId?: InputMaybe<IntFilter>;
}

export interface DateTimeFieldUpdateOperationsInput {
  set?: InputMaybe<Scalars["DateTime"]["input"]>;
}

export interface DateTimeFilter {
  equals?: InputMaybe<Scalars["DateTime"]["input"]>;
  gt?: InputMaybe<Scalars["DateTime"]["input"]>;
  gte?: InputMaybe<Scalars["DateTime"]["input"]>;
  in?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
  lt?: InputMaybe<Scalars["DateTime"]["input"]>;
  lte?: InputMaybe<Scalars["DateTime"]["input"]>;
  not?: InputMaybe<NestedDateTimeFilter>;
  notIn?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
}

export interface DateTimeNullableFilter {
  equals?: InputMaybe<Scalars["DateTime"]["input"]>;
  gt?: InputMaybe<Scalars["DateTime"]["input"]>;
  gte?: InputMaybe<Scalars["DateTime"]["input"]>;
  in?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
  lt?: InputMaybe<Scalars["DateTime"]["input"]>;
  lte?: InputMaybe<Scalars["DateTime"]["input"]>;
  not?: InputMaybe<NestedDateTimeNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
}

export interface DateTimeNullableWithAggregatesFilter {
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedDateTimeNullableFilter>;
  _min?: InputMaybe<NestedDateTimeNullableFilter>;
  equals?: InputMaybe<Scalars["DateTime"]["input"]>;
  gt?: InputMaybe<Scalars["DateTime"]["input"]>;
  gte?: InputMaybe<Scalars["DateTime"]["input"]>;
  in?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
  lt?: InputMaybe<Scalars["DateTime"]["input"]>;
  lte?: InputMaybe<Scalars["DateTime"]["input"]>;
  not?: InputMaybe<NestedDateTimeNullableWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
}

export interface DateTimeWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedDateTimeFilter>;
  _min?: InputMaybe<NestedDateTimeFilter>;
  equals?: InputMaybe<Scalars["DateTime"]["input"]>;
  gt?: InputMaybe<Scalars["DateTime"]["input"]>;
  gte?: InputMaybe<Scalars["DateTime"]["input"]>;
  in?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
  lt?: InputMaybe<Scalars["DateTime"]["input"]>;
  lte?: InputMaybe<Scalars["DateTime"]["input"]>;
  not?: InputMaybe<NestedDateTimeWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
}

export interface EnumTransferTypeFieldUpdateOperationsInput {
  set?: InputMaybe<TransferType>;
}

export interface EnumTransferTypeFilter {
  equals?: InputMaybe<TransferType>;
  in?: InputMaybe<TransferType[]>;
  not?: InputMaybe<TransferType>;
  notIn?: InputMaybe<TransferType[]>;
}

export interface EnumTransferTypeWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedEnumTransferTypeFilter>;
  _min?: InputMaybe<NestedEnumTransferTypeFilter>;
  equals?: InputMaybe<TransferType>;
  in?: InputMaybe<TransferType[]>;
  not?: InputMaybe<TransferType>;
  notIn?: InputMaybe<TransferType[]>;
}

export interface FloatFieldUpdateOperationsInput {
  decrement?: InputMaybe<Scalars["Float"]["input"]>;
  divide?: InputMaybe<Scalars["Float"]["input"]>;
  increment?: InputMaybe<Scalars["Float"]["input"]>;
  multiply?: InputMaybe<Scalars["Float"]["input"]>;
  set?: InputMaybe<Scalars["Float"]["input"]>;
}

export interface FloatFilter {
  equals?: InputMaybe<Scalars["Float"]["input"]>;
  gt?: InputMaybe<Scalars["Float"]["input"]>;
  gte?: InputMaybe<Scalars["Float"]["input"]>;
  in?: InputMaybe<Array<Scalars["Float"]["input"]>>;
  lt?: InputMaybe<Scalars["Float"]["input"]>;
  lte?: InputMaybe<Scalars["Float"]["input"]>;
  not?: InputMaybe<NestedFloatFilter>;
  notIn?: InputMaybe<Array<Scalars["Float"]["input"]>>;
}

export interface FloatWithAggregatesFilter {
  _avg?: InputMaybe<NestedFloatFilter>;
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedFloatFilter>;
  _min?: InputMaybe<NestedFloatFilter>;
  _sum?: InputMaybe<NestedFloatFilter>;
  equals?: InputMaybe<Scalars["Float"]["input"]>;
  gt?: InputMaybe<Scalars["Float"]["input"]>;
  gte?: InputMaybe<Scalars["Float"]["input"]>;
  in?: InputMaybe<Array<Scalars["Float"]["input"]>>;
  lt?: InputMaybe<Scalars["Float"]["input"]>;
  lte?: InputMaybe<Scalars["Float"]["input"]>;
  not?: InputMaybe<NestedFloatWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["Float"]["input"]>>;
}

export interface GroundStation {
  __typename?: "GroundStation";
  description: Scalars["String"]["output"];
  id: Scalars["ID"]["output"];
  lat: Scalars["Float"]["output"];
  lng: Scalars["Float"]["output"];
  minElevation: Scalars["Float"]["output"];
  name: Scalars["String"]["output"];
  passSchedule: Pass[];
}

export interface GroundStationPassScheduleArgs {
  cursor?: InputMaybe<PassWhereUniqueInput>;
  distinct?: InputMaybe<PassScalarFieldEnum[]>;
  orderBy?: InputMaybe<PassOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PassWhereInput>;
}

export interface GroundStationAvgOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  lat?: InputMaybe<SortOrder>;
  lng?: InputMaybe<SortOrder>;
  minElevation?: InputMaybe<SortOrder>;
}

export interface GroundStationCountOrderByAggregateInput {
  description?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  lat?: InputMaybe<SortOrder>;
  lng?: InputMaybe<SortOrder>;
  minElevation?: InputMaybe<SortOrder>;
  name?: InputMaybe<SortOrder>;
}

export interface GroundStationCreateInput {
  description: Scalars["String"]["input"];
  lat: Scalars["Float"]["input"];
  lng: Scalars["Float"]["input"];
  minElevation: Scalars["Float"]["input"];
  name: Scalars["String"]["input"];
  passSchedule?: InputMaybe<PassCreateNestedManyWithoutGroundStationInput>;
}

export interface GroundStationCreateManyInput {
  description: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["Int"]["input"]>;
  lat: Scalars["Float"]["input"];
  lng: Scalars["Float"]["input"];
  minElevation: Scalars["Float"]["input"];
  name: Scalars["String"]["input"];
}

export interface GroundStationCreateNestedOneWithoutPassScheduleInput {
  connect?: InputMaybe<GroundStationWhereUniqueInput>;
  connectOrCreate?: InputMaybe<GroundStationCreateOrConnectWithoutPassScheduleInput>;
  create?: InputMaybe<GroundStationCreateWithoutPassScheduleInput>;
}

export interface GroundStationCreateOrConnectWithoutPassScheduleInput {
  create: GroundStationCreateWithoutPassScheduleInput;
  where: GroundStationWhereUniqueInput;
}

export interface GroundStationCreateWithoutPassScheduleInput {
  description: Scalars["String"]["input"];
  lat: Scalars["Float"]["input"];
  lng: Scalars["Float"]["input"];
  minElevation: Scalars["Float"]["input"];
  name: Scalars["String"]["input"];
}

export interface GroundStationMaxOrderByAggregateInput {
  description?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  lat?: InputMaybe<SortOrder>;
  lng?: InputMaybe<SortOrder>;
  minElevation?: InputMaybe<SortOrder>;
  name?: InputMaybe<SortOrder>;
}

export interface GroundStationMinOrderByAggregateInput {
  description?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  lat?: InputMaybe<SortOrder>;
  lng?: InputMaybe<SortOrder>;
  minElevation?: InputMaybe<SortOrder>;
  name?: InputMaybe<SortOrder>;
}

export interface GroundStationOrderByWithAggregationInput {
  _avg?: InputMaybe<GroundStationAvgOrderByAggregateInput>;
  _count?: InputMaybe<GroundStationCountOrderByAggregateInput>;
  _max?: InputMaybe<GroundStationMaxOrderByAggregateInput>;
  _min?: InputMaybe<GroundStationMinOrderByAggregateInput>;
  _sum?: InputMaybe<GroundStationSumOrderByAggregateInput>;
  description?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  lat?: InputMaybe<SortOrder>;
  lng?: InputMaybe<SortOrder>;
  minElevation?: InputMaybe<SortOrder>;
  name?: InputMaybe<SortOrder>;
}

export interface GroundStationOrderByWithRelationInput {
  description?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  lat?: InputMaybe<SortOrder>;
  lng?: InputMaybe<SortOrder>;
  minElevation?: InputMaybe<SortOrder>;
  name?: InputMaybe<SortOrder>;
  passSchedule?: InputMaybe<PassOrderByRelationAggregateInput>;
}

export interface GroundStationRelationFilter {
  is?: InputMaybe<GroundStationWhereInput>;
  isNot?: InputMaybe<GroundStationWhereInput>;
}

export enum GroundStationScalarFieldEnum {
  Description = "description",
  Id = "id",
  Lat = "lat",
  Lng = "lng",
  MinElevation = "minElevation",
  Name = "name",
}

export interface GroundStationScalarWhereWithAggregatesInput {
  AND?: InputMaybe<GroundStationScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<GroundStationScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<GroundStationScalarWhereWithAggregatesInput[]>;
  description?: InputMaybe<StringWithAggregatesFilter>;
  id?: InputMaybe<IntWithAggregatesFilter>;
  lat?: InputMaybe<FloatWithAggregatesFilter>;
  lng?: InputMaybe<FloatWithAggregatesFilter>;
  minElevation?: InputMaybe<FloatWithAggregatesFilter>;
  name?: InputMaybe<StringWithAggregatesFilter>;
}

export interface GroundStationSumOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  lat?: InputMaybe<SortOrder>;
  lng?: InputMaybe<SortOrder>;
  minElevation?: InputMaybe<SortOrder>;
}

export interface GroundStationUpdateInput {
  description?: InputMaybe<StringFieldUpdateOperationsInput>;
  lat?: InputMaybe<FloatFieldUpdateOperationsInput>;
  lng?: InputMaybe<FloatFieldUpdateOperationsInput>;
  minElevation?: InputMaybe<FloatFieldUpdateOperationsInput>;
  name?: InputMaybe<StringFieldUpdateOperationsInput>;
  passSchedule?: InputMaybe<PassUpdateManyWithoutGroundStationNestedInput>;
}

export interface GroundStationUpdateManyMutationInput {
  description?: InputMaybe<StringFieldUpdateOperationsInput>;
  lat?: InputMaybe<FloatFieldUpdateOperationsInput>;
  lng?: InputMaybe<FloatFieldUpdateOperationsInput>;
  minElevation?: InputMaybe<FloatFieldUpdateOperationsInput>;
  name?: InputMaybe<StringFieldUpdateOperationsInput>;
}

export interface GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput {
  connect?: InputMaybe<GroundStationWhereUniqueInput>;
  connectOrCreate?: InputMaybe<GroundStationCreateOrConnectWithoutPassScheduleInput>;
  create?: InputMaybe<GroundStationCreateWithoutPassScheduleInput>;
  update?: InputMaybe<GroundStationUpdateToOneWithWhereWithoutPassScheduleInput>;
  upsert?: InputMaybe<GroundStationUpsertWithoutPassScheduleInput>;
}

export interface GroundStationUpdateToOneWithWhereWithoutPassScheduleInput {
  data: GroundStationUpdateWithoutPassScheduleInput;
  where?: InputMaybe<GroundStationWhereInput>;
}

export interface GroundStationUpdateWithoutPassScheduleInput {
  description?: InputMaybe<StringFieldUpdateOperationsInput>;
  lat?: InputMaybe<FloatFieldUpdateOperationsInput>;
  lng?: InputMaybe<FloatFieldUpdateOperationsInput>;
  minElevation?: InputMaybe<FloatFieldUpdateOperationsInput>;
  name?: InputMaybe<StringFieldUpdateOperationsInput>;
}

export interface GroundStationUpsertWithoutPassScheduleInput {
  create: GroundStationCreateWithoutPassScheduleInput;
  update: GroundStationUpdateWithoutPassScheduleInput;
  where?: InputMaybe<GroundStationWhereInput>;
}

export interface GroundStationWhereInput {
  AND?: InputMaybe<GroundStationWhereInput[]>;
  NOT?: InputMaybe<GroundStationWhereInput[]>;
  OR?: InputMaybe<GroundStationWhereInput[]>;
  description?: InputMaybe<StringFilter>;
  id?: InputMaybe<IntFilter>;
  lat?: InputMaybe<FloatFilter>;
  lng?: InputMaybe<FloatFilter>;
  minElevation?: InputMaybe<FloatFilter>;
  name?: InputMaybe<StringFilter>;
  passSchedule?: InputMaybe<PassListRelationFilter>;
}

export interface GroundStationWhereUniqueInput {
  AND?: InputMaybe<GroundStationWhereInput[]>;
  NOT?: InputMaybe<GroundStationWhereInput[]>;
  OR?: InputMaybe<GroundStationWhereInput[]>;
  description?: InputMaybe<StringFilter>;
  id?: InputMaybe<Scalars["Int"]["input"]>;
  lat?: InputMaybe<FloatFilter>;
  lng?: InputMaybe<FloatFilter>;
  minElevation?: InputMaybe<FloatFilter>;
  name?: InputMaybe<StringFilter>;
  passSchedule?: InputMaybe<PassListRelationFilter>;
}

export interface IntFieldUpdateOperationsInput {
  decrement?: InputMaybe<Scalars["Int"]["input"]>;
  divide?: InputMaybe<Scalars["Int"]["input"]>;
  increment?: InputMaybe<Scalars["Int"]["input"]>;
  multiply?: InputMaybe<Scalars["Int"]["input"]>;
  set?: InputMaybe<Scalars["Int"]["input"]>;
}

export interface IntFilter {
  equals?: InputMaybe<Scalars["Int"]["input"]>;
  gt?: InputMaybe<Scalars["Int"]["input"]>;
  gte?: InputMaybe<Scalars["Int"]["input"]>;
  in?: InputMaybe<Array<Scalars["Int"]["input"]>>;
  lt?: InputMaybe<Scalars["Int"]["input"]>;
  lte?: InputMaybe<Scalars["Int"]["input"]>;
  not?: InputMaybe<NestedIntFilter>;
  notIn?: InputMaybe<Array<Scalars["Int"]["input"]>>;
}

export interface IntNullableFilter {
  equals?: InputMaybe<Scalars["Int"]["input"]>;
  gt?: InputMaybe<Scalars["Int"]["input"]>;
  gte?: InputMaybe<Scalars["Int"]["input"]>;
  in?: InputMaybe<Array<Scalars["Int"]["input"]>>;
  lt?: InputMaybe<Scalars["Int"]["input"]>;
  lte?: InputMaybe<Scalars["Int"]["input"]>;
  not?: InputMaybe<NestedIntNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["Int"]["input"]>>;
}

export interface IntNullableWithAggregatesFilter {
  _avg?: InputMaybe<NestedFloatNullableFilter>;
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedIntNullableFilter>;
  _min?: InputMaybe<NestedIntNullableFilter>;
  _sum?: InputMaybe<NestedIntNullableFilter>;
  equals?: InputMaybe<Scalars["Int"]["input"]>;
  gt?: InputMaybe<Scalars["Int"]["input"]>;
  gte?: InputMaybe<Scalars["Int"]["input"]>;
  in?: InputMaybe<Array<Scalars["Int"]["input"]>>;
  lt?: InputMaybe<Scalars["Int"]["input"]>;
  lte?: InputMaybe<Scalars["Int"]["input"]>;
  not?: InputMaybe<NestedIntNullableWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["Int"]["input"]>>;
}

export interface IntWithAggregatesFilter {
  _avg?: InputMaybe<NestedFloatFilter>;
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedIntFilter>;
  _min?: InputMaybe<NestedIntFilter>;
  _sum?: InputMaybe<NestedIntFilter>;
  equals?: InputMaybe<Scalars["Int"]["input"]>;
  gt?: InputMaybe<Scalars["Int"]["input"]>;
  gte?: InputMaybe<Scalars["Int"]["input"]>;
  in?: InputMaybe<Array<Scalars["Int"]["input"]>>;
  lt?: InputMaybe<Scalars["Int"]["input"]>;
  lte?: InputMaybe<Scalars["Int"]["input"]>;
  not?: InputMaybe<NestedIntWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["Int"]["input"]>>;
}

export interface JsonFilter {
  array_contains?: InputMaybe<Scalars["Json"]["input"]>;
  array_ends_with?: InputMaybe<Scalars["Json"]["input"]>;
  array_starts_with?: InputMaybe<Scalars["Json"]["input"]>;
  equals?: InputMaybe<Scalars["Json"]["input"]>;
  gt?: InputMaybe<Scalars["Json"]["input"]>;
  gte?: InputMaybe<Scalars["Json"]["input"]>;
  lt?: InputMaybe<Scalars["Json"]["input"]>;
  lte?: InputMaybe<Scalars["Json"]["input"]>;
  not?: InputMaybe<Scalars["Json"]["input"]>;
  path?: InputMaybe<Array<Scalars["String"]["input"]>>;
  string_contains?: InputMaybe<Scalars["String"]["input"]>;
  string_ends_with?: InputMaybe<Scalars["String"]["input"]>;
  string_starts_with?: InputMaybe<Scalars["String"]["input"]>;
}

export enum JsonNullValueFilter {
  AnyNull = "AnyNull",
  DbNull = "DbNull",
  JsonNull = "JsonNull",
}

export enum JsonNullValueInput {
  JsonNull = "JsonNull",
}

export interface JsonWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedJsonFilter>;
  _min?: InputMaybe<NestedJsonFilter>;
  array_contains?: InputMaybe<Scalars["Json"]["input"]>;
  array_ends_with?: InputMaybe<Scalars["Json"]["input"]>;
  array_starts_with?: InputMaybe<Scalars["Json"]["input"]>;
  equals?: InputMaybe<Scalars["Json"]["input"]>;
  gt?: InputMaybe<Scalars["Json"]["input"]>;
  gte?: InputMaybe<Scalars["Json"]["input"]>;
  lt?: InputMaybe<Scalars["Json"]["input"]>;
  lte?: InputMaybe<Scalars["Json"]["input"]>;
  not?: InputMaybe<Scalars["Json"]["input"]>;
  path?: InputMaybe<Array<Scalars["String"]["input"]>>;
  string_contains?: InputMaybe<Scalars["String"]["input"]>;
  string_ends_with?: InputMaybe<Scalars["String"]["input"]>;
  string_starts_with?: InputMaybe<Scalars["String"]["input"]>;
}

export interface Log {
  __typename?: "Log";
  command: Scalars["String"]["output"];
  id: Scalars["ID"]["output"];
  ranAt: Scalars["DateTime"]["output"];
  user: User;
  userId: Scalars["Int"]["output"];
}

export interface LogAvgOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface LogCountOrderByAggregateInput {
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  ranAt?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface LogCreateInput {
  command: Scalars["String"]["input"];
  ranAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  user: UserCreateNestedOneWithoutLogInput;
}

export interface LogCreateManyInput {
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["Int"]["input"]>;
  ranAt?: InputMaybe<Scalars["DateTime"]["input"]>;
  userId: Scalars["Int"]["input"];
}

export interface LogCreateManyUserInput {
  command: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["Int"]["input"]>;
  ranAt?: InputMaybe<Scalars["DateTime"]["input"]>;
}

export interface LogCreateManyUserInputEnvelope {
  data: LogCreateManyUserInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface LogCreateNestedManyWithoutUserInput {
  connect?: InputMaybe<LogWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<LogCreateOrConnectWithoutUserInput[]>;
  create?: InputMaybe<LogCreateWithoutUserInput[]>;
  createMany?: InputMaybe<LogCreateManyUserInputEnvelope>;
}

export interface LogCreateOrConnectWithoutUserInput {
  create: LogCreateWithoutUserInput;
  where: LogWhereUniqueInput;
}

export interface LogCreateWithoutUserInput {
  command: Scalars["String"]["input"];
  ranAt?: InputMaybe<Scalars["DateTime"]["input"]>;
}

export interface LogListRelationFilter {
  every?: InputMaybe<LogWhereInput>;
  none?: InputMaybe<LogWhereInput>;
  some?: InputMaybe<LogWhereInput>;
}

export interface LogMaxOrderByAggregateInput {
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  ranAt?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface LogMinOrderByAggregateInput {
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  ranAt?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface LogOrderByRelationAggregateInput {
  _count?: InputMaybe<SortOrder>;
}

export interface LogOrderByWithAggregationInput {
  _avg?: InputMaybe<LogAvgOrderByAggregateInput>;
  _count?: InputMaybe<LogCountOrderByAggregateInput>;
  _max?: InputMaybe<LogMaxOrderByAggregateInput>;
  _min?: InputMaybe<LogMinOrderByAggregateInput>;
  _sum?: InputMaybe<LogSumOrderByAggregateInput>;
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  ranAt?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface LogOrderByWithRelationInput {
  command?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  ranAt?: InputMaybe<SortOrder>;
  user?: InputMaybe<UserOrderByWithRelationInput>;
  userId?: InputMaybe<SortOrder>;
}

export enum LogScalarFieldEnum {
  Command = "command",
  Id = "id",
  RanAt = "ranAt",
  UserId = "userId",
}

export interface LogScalarWhereInput {
  AND?: InputMaybe<LogScalarWhereInput[]>;
  NOT?: InputMaybe<LogScalarWhereInput[]>;
  OR?: InputMaybe<LogScalarWhereInput[]>;
  command?: InputMaybe<StringFilter>;
  id?: InputMaybe<IntFilter>;
  ranAt?: InputMaybe<DateTimeFilter>;
  userId?: InputMaybe<IntFilter>;
}

export interface LogScalarWhereWithAggregatesInput {
  AND?: InputMaybe<LogScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<LogScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<LogScalarWhereWithAggregatesInput[]>;
  command?: InputMaybe<StringWithAggregatesFilter>;
  id?: InputMaybe<IntWithAggregatesFilter>;
  ranAt?: InputMaybe<DateTimeWithAggregatesFilter>;
  userId?: InputMaybe<IntWithAggregatesFilter>;
}

export interface LogSumOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  userId?: InputMaybe<SortOrder>;
}

export interface LogUpdateInput {
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  ranAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
  user?: InputMaybe<UserUpdateOneRequiredWithoutLogNestedInput>;
}

export interface LogUpdateManyMutationInput {
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  ranAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
}

export interface LogUpdateManyWithWhereWithoutUserInput {
  data: LogUpdateManyMutationInput;
  where: LogScalarWhereInput;
}

export interface LogUpdateManyWithoutUserNestedInput {
  connect?: InputMaybe<LogWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<LogCreateOrConnectWithoutUserInput[]>;
  create?: InputMaybe<LogCreateWithoutUserInput[]>;
  createMany?: InputMaybe<LogCreateManyUserInputEnvelope>;
  delete?: InputMaybe<LogWhereUniqueInput[]>;
  deleteMany?: InputMaybe<LogScalarWhereInput[]>;
  disconnect?: InputMaybe<LogWhereUniqueInput[]>;
  set?: InputMaybe<LogWhereUniqueInput[]>;
  update?: InputMaybe<LogUpdateWithWhereUniqueWithoutUserInput[]>;
  updateMany?: InputMaybe<LogUpdateManyWithWhereWithoutUserInput[]>;
  upsert?: InputMaybe<LogUpsertWithWhereUniqueWithoutUserInput[]>;
}

export interface LogUpdateWithWhereUniqueWithoutUserInput {
  data: LogUpdateWithoutUserInput;
  where: LogWhereUniqueInput;
}

export interface LogUpdateWithoutUserInput {
  command?: InputMaybe<StringFieldUpdateOperationsInput>;
  ranAt?: InputMaybe<DateTimeFieldUpdateOperationsInput>;
}

export interface LogUpsertWithWhereUniqueWithoutUserInput {
  create: LogCreateWithoutUserInput;
  update: LogUpdateWithoutUserInput;
  where: LogWhereUniqueInput;
}

export interface LogWhereInput {
  AND?: InputMaybe<LogWhereInput[]>;
  NOT?: InputMaybe<LogWhereInput[]>;
  OR?: InputMaybe<LogWhereInput[]>;
  command?: InputMaybe<StringFilter>;
  id?: InputMaybe<IntFilter>;
  ranAt?: InputMaybe<DateTimeFilter>;
  user?: InputMaybe<UserWhereInput>;
  userId?: InputMaybe<IntFilter>;
}

export interface LogWhereUniqueInput {
  AND?: InputMaybe<LogWhereInput[]>;
  NOT?: InputMaybe<LogWhereInput[]>;
  OR?: InputMaybe<LogWhereInput[]>;
  command?: InputMaybe<StringFilter>;
  id?: InputMaybe<Scalars["Int"]["input"]>;
  ranAt?: InputMaybe<DateTimeFilter>;
  user?: InputMaybe<UserWhereInput>;
  userId?: InputMaybe<IntFilter>;
}

export interface Mutation {
  __typename?: "Mutation";
  createManyActualRiseSet: ActualRiseSet[];
  createManyAdmin: Admin[];
  createManyCommand: Command[];
  createManyGroundStation: GroundStation[];
  createManyLog: Log[];
  createManyPass: Pass[];
  createManyPredictedRiseSet: PredictedRiseSet[];
  createManyResult: Result[];
  createManyStateElement: StateElement[];
  createManyTransfer: Transfer[];
  createManyUser: User[];
  createOneActualRiseSet: ActualRiseSet;
  createOneAdmin: Admin;
  createOneCommand: Command;
  createOneGroundStation: GroundStation;
  createOneLog: Log;
  createOnePass: Pass;
  createOnePredictedRiseSet: PredictedRiseSet;
  createOneResult: Result;
  createOneStateElement: StateElement;
  createOneTransfer: Transfer;
  createOneUser: User;
  deleteManyActualRiseSet?: Maybe<BatchPayload>;
  deleteManyAdmin?: Maybe<BatchPayload>;
  deleteManyCommand?: Maybe<BatchPayload>;
  deleteManyGroundStation?: Maybe<BatchPayload>;
  deleteManyLog?: Maybe<BatchPayload>;
  deleteManyPass?: Maybe<BatchPayload>;
  deleteManyPredictedRiseSet?: Maybe<BatchPayload>;
  deleteManyResult?: Maybe<BatchPayload>;
  deleteManyStateElement?: Maybe<BatchPayload>;
  deleteManyTransfer?: Maybe<BatchPayload>;
  deleteManyUser?: Maybe<BatchPayload>;
  deleteOneActualRiseSet?: Maybe<ActualRiseSet>;
  deleteOneAdmin?: Maybe<Admin>;
  deleteOneCommand?: Maybe<Command>;
  deleteOneGroundStation?: Maybe<GroundStation>;
  deleteOneLog?: Maybe<Log>;
  deleteOnePass?: Maybe<Pass>;
  deleteOnePredictedRiseSet?: Maybe<PredictedRiseSet>;
  deleteOneResult?: Maybe<Result>;
  deleteOneStateElement?: Maybe<StateElement>;
  deleteOneTransfer?: Maybe<Transfer>;
  deleteOneUser?: Maybe<User>;
  passSchedulingUpdatePass: Scalars["Boolean"]["output"];
  updateManyActualRiseSet: BatchPayload;
  updateManyAdmin: BatchPayload;
  updateManyCommand: BatchPayload;
  updateManyGroundStation: BatchPayload;
  updateManyLog: BatchPayload;
  updateManyPass: BatchPayload;
  updateManyPredictedRiseSet: BatchPayload;
  updateManyResult: BatchPayload;
  updateManyStateElement: BatchPayload;
  updateManyTransfer: BatchPayload;
  updateManyUser: BatchPayload;
  updateOneActualRiseSet?: Maybe<ActualRiseSet>;
  updateOneAdmin?: Maybe<Admin>;
  updateOneCommand?: Maybe<Command>;
  updateOneGroundStation?: Maybe<GroundStation>;
  updateOneLog?: Maybe<Log>;
  updateOnePass?: Maybe<Pass>;
  updateOnePredictedRiseSet?: Maybe<PredictedRiseSet>;
  updateOneResult?: Maybe<Result>;
  updateOneStateElement?: Maybe<StateElement>;
  updateOneTransfer?: Maybe<Transfer>;
  updateOneUser?: Maybe<User>;
  upsertOneActualRiseSet: ActualRiseSet;
  upsertOneAdmin: Admin;
  upsertOneCommand: Command;
  upsertOneGroundStation: GroundStation;
  upsertOneLog: Log;
  upsertOnePass: Pass;
  upsertOnePredictedRiseSet: PredictedRiseSet;
  upsertOneResult: Result;
  upsertOneStateElement: StateElement;
  upsertOneTransfer: Transfer;
  upsertOneUser: User;
}

export interface MutationCreateManyActualRiseSetArgs {
  data: ActualRiseSetCreateInput[];
}

export interface MutationCreateManyAdminArgs {
  data: AdminCreateInput[];
}

export interface MutationCreateManyCommandArgs {
  data: CommandCreateInput[];
}

export interface MutationCreateManyGroundStationArgs {
  data: GroundStationCreateInput[];
}

export interface MutationCreateManyLogArgs {
  data: LogCreateInput[];
}

export interface MutationCreateManyPassArgs {
  data: PassCreateInput[];
}

export interface MutationCreateManyPredictedRiseSetArgs {
  data: PredictedRiseSetCreateInput[];
}

export interface MutationCreateManyResultArgs {
  data: ResultCreateInput[];
}

export interface MutationCreateManyStateElementArgs {
  data: StateElementCreateInput[];
}

export interface MutationCreateManyTransferArgs {
  data: TransferCreateInput[];
}

export interface MutationCreateManyUserArgs {
  data: UserCreateInput[];
}

export interface MutationCreateOneActualRiseSetArgs {
  data: ActualRiseSetCreateInput;
}

export interface MutationCreateOneAdminArgs {
  data: AdminCreateInput;
}

export interface MutationCreateOneCommandArgs {
  data: CommandCreateInput;
}

export interface MutationCreateOneGroundStationArgs {
  data: GroundStationCreateInput;
}

export interface MutationCreateOneLogArgs {
  data: LogCreateInput;
}

export interface MutationCreateOnePassArgs {
  data: PassCreateInput;
}

export interface MutationCreateOnePredictedRiseSetArgs {
  data: PredictedRiseSetCreateInput;
}

export interface MutationCreateOneResultArgs {
  data: ResultCreateInput;
}

export interface MutationCreateOneStateElementArgs {
  data: StateElementCreateInput;
}

export interface MutationCreateOneTransferArgs {
  data: TransferCreateInput;
}

export interface MutationCreateOneUserArgs {
  data: UserCreateInput;
}

export interface MutationDeleteManyActualRiseSetArgs {
  where: ActualRiseSetWhereInput;
}

export interface MutationDeleteManyAdminArgs {
  where: AdminWhereInput;
}

export interface MutationDeleteManyCommandArgs {
  where: CommandWhereInput;
}

export interface MutationDeleteManyGroundStationArgs {
  where: GroundStationWhereInput;
}

export interface MutationDeleteManyLogArgs {
  where: LogWhereInput;
}

export interface MutationDeleteManyPassArgs {
  where: PassWhereInput;
}

export interface MutationDeleteManyPredictedRiseSetArgs {
  where: PredictedRiseSetWhereInput;
}

export interface MutationDeleteManyResultArgs {
  where: ResultWhereInput;
}

export interface MutationDeleteManyStateElementArgs {
  where: StateElementWhereInput;
}

export interface MutationDeleteManyTransferArgs {
  where: TransferWhereInput;
}

export interface MutationDeleteManyUserArgs {
  where: UserWhereInput;
}

export interface MutationDeleteOneActualRiseSetArgs {
  where: ActualRiseSetWhereUniqueInput;
}

export interface MutationDeleteOneAdminArgs {
  where: AdminWhereUniqueInput;
}

export interface MutationDeleteOneCommandArgs {
  where: CommandWhereUniqueInput;
}

export interface MutationDeleteOneGroundStationArgs {
  where: GroundStationWhereUniqueInput;
}

export interface MutationDeleteOneLogArgs {
  where: LogWhereUniqueInput;
}

export interface MutationDeleteOnePassArgs {
  where: PassWhereUniqueInput;
}

export interface MutationDeleteOnePredictedRiseSetArgs {
  where: PredictedRiseSetWhereUniqueInput;
}

export interface MutationDeleteOneResultArgs {
  where: ResultWhereUniqueInput;
}

export interface MutationDeleteOneStateElementArgs {
  where: StateElementWhereUniqueInput;
}

export interface MutationDeleteOneTransferArgs {
  where: TransferWhereUniqueInput;
}

export interface MutationDeleteOneUserArgs {
  where: UserWhereUniqueInput;
}

export interface MutationPassSchedulingUpdatePassArgs {
  append?: InputMaybe<PassSchedulingUpdatePassAppendInput[]>;
  delete?: InputMaybe<PassSchedulingUpdatePassDeleteInput[]>;
  insert?: InputMaybe<PassSchedulingUpdatePassInsertInput[]>;
  update?: InputMaybe<PassSchedulingUpdatePassUpdateInput[]>;
}

export interface MutationUpdateManyActualRiseSetArgs {
  data: ActualRiseSetUpdateManyMutationInput;
  where?: InputMaybe<ActualRiseSetWhereInput>;
}

export interface MutationUpdateManyAdminArgs {
  data: AdminUpdateManyMutationInput;
  where?: InputMaybe<AdminWhereInput>;
}

export interface MutationUpdateManyCommandArgs {
  data: CommandUpdateManyMutationInput;
  where?: InputMaybe<CommandWhereInput>;
}

export interface MutationUpdateManyGroundStationArgs {
  data: GroundStationUpdateManyMutationInput;
  where?: InputMaybe<GroundStationWhereInput>;
}

export interface MutationUpdateManyLogArgs {
  data: LogUpdateManyMutationInput;
  where?: InputMaybe<LogWhereInput>;
}

export interface MutationUpdateManyPassArgs {
  data: PassUpdateManyMutationInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface MutationUpdateManyPredictedRiseSetArgs {
  data: PredictedRiseSetUpdateManyMutationInput;
  where?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface MutationUpdateManyResultArgs {
  data: ResultUpdateManyMutationInput;
  where?: InputMaybe<ResultWhereInput>;
}

export interface MutationUpdateManyStateElementArgs {
  data: StateElementUpdateManyMutationInput;
  where?: InputMaybe<StateElementWhereInput>;
}

export interface MutationUpdateManyTransferArgs {
  data: TransferUpdateManyMutationInput;
  where?: InputMaybe<TransferWhereInput>;
}

export interface MutationUpdateManyUserArgs {
  data: UserUpdateManyMutationInput;
  where?: InputMaybe<UserWhereInput>;
}

export interface MutationUpdateOneActualRiseSetArgs {
  data: ActualRiseSetUpdateInput;
  where: ActualRiseSetWhereUniqueInput;
}

export interface MutationUpdateOneAdminArgs {
  data: AdminUpdateInput;
  where: AdminWhereUniqueInput;
}

export interface MutationUpdateOneCommandArgs {
  data: CommandUpdateInput;
  where: CommandWhereUniqueInput;
}

export interface MutationUpdateOneGroundStationArgs {
  data: GroundStationUpdateInput;
  where: GroundStationWhereUniqueInput;
}

export interface MutationUpdateOneLogArgs {
  data: LogUpdateInput;
  where: LogWhereUniqueInput;
}

export interface MutationUpdateOnePassArgs {
  data: PassUpdateInput;
  where: PassWhereUniqueInput;
}

export interface MutationUpdateOnePredictedRiseSetArgs {
  data: PredictedRiseSetUpdateInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface MutationUpdateOneResultArgs {
  data: ResultUpdateInput;
  where: ResultWhereUniqueInput;
}

export interface MutationUpdateOneStateElementArgs {
  data: StateElementUpdateInput;
  where: StateElementWhereUniqueInput;
}

export interface MutationUpdateOneTransferArgs {
  data: TransferUpdateInput;
  where: TransferWhereUniqueInput;
}

export interface MutationUpdateOneUserArgs {
  data: UserUpdateInput;
  where: UserWhereUniqueInput;
}

export interface MutationUpsertOneActualRiseSetArgs {
  create: ActualRiseSetCreateInput;
  update: ActualRiseSetUpdateInput;
  where: ActualRiseSetWhereUniqueInput;
}

export interface MutationUpsertOneAdminArgs {
  create: AdminCreateInput;
  update: AdminUpdateInput;
  where: AdminWhereUniqueInput;
}

export interface MutationUpsertOneCommandArgs {
  create: CommandCreateInput;
  update: CommandUpdateInput;
  where: CommandWhereUniqueInput;
}

export interface MutationUpsertOneGroundStationArgs {
  create: GroundStationCreateInput;
  update: GroundStationUpdateInput;
  where: GroundStationWhereUniqueInput;
}

export interface MutationUpsertOneLogArgs {
  create: LogCreateInput;
  update: LogUpdateInput;
  where: LogWhereUniqueInput;
}

export interface MutationUpsertOnePassArgs {
  create: PassCreateInput;
  update: PassUpdateInput;
  where: PassWhereUniqueInput;
}

export interface MutationUpsertOnePredictedRiseSetArgs {
  create: PredictedRiseSetCreateInput;
  update: PredictedRiseSetUpdateInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface MutationUpsertOneResultArgs {
  create: ResultCreateInput;
  update: ResultUpdateInput;
  where: ResultWhereUniqueInput;
}

export interface MutationUpsertOneStateElementArgs {
  create: StateElementCreateInput;
  update: StateElementUpdateInput;
  where: StateElementWhereUniqueInput;
}

export interface MutationUpsertOneTransferArgs {
  create: TransferCreateInput;
  update: TransferUpdateInput;
  where: TransferWhereUniqueInput;
}

export interface MutationUpsertOneUserArgs {
  create: UserCreateInput;
  update: UserUpdateInput;
  where: UserWhereUniqueInput;
}

export interface NestedBigIntFilter {
  equals?: InputMaybe<Scalars["BigInt"]["input"]>;
  gt?: InputMaybe<Scalars["BigInt"]["input"]>;
  gte?: InputMaybe<Scalars["BigInt"]["input"]>;
  in?: InputMaybe<Array<Scalars["BigInt"]["input"]>>;
  lt?: InputMaybe<Scalars["BigInt"]["input"]>;
  lte?: InputMaybe<Scalars["BigInt"]["input"]>;
  not?: InputMaybe<NestedBigIntFilter>;
  notIn?: InputMaybe<Array<Scalars["BigInt"]["input"]>>;
}

export interface NestedBigIntWithAggregatesFilter {
  _avg?: InputMaybe<NestedFloatFilter>;
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedBigIntFilter>;
  _min?: InputMaybe<NestedBigIntFilter>;
  _sum?: InputMaybe<NestedBigIntFilter>;
  equals?: InputMaybe<Scalars["BigInt"]["input"]>;
  gt?: InputMaybe<Scalars["BigInt"]["input"]>;
  gte?: InputMaybe<Scalars["BigInt"]["input"]>;
  in?: InputMaybe<Array<Scalars["BigInt"]["input"]>>;
  lt?: InputMaybe<Scalars["BigInt"]["input"]>;
  lte?: InputMaybe<Scalars["BigInt"]["input"]>;
  not?: InputMaybe<NestedBigIntWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["BigInt"]["input"]>>;
}

export interface NestedBoolFilter {
  equals?: InputMaybe<Scalars["Boolean"]["input"]>;
  not?: InputMaybe<NestedBoolFilter>;
}

export interface NestedBoolNullableFilter {
  equals?: InputMaybe<Scalars["Boolean"]["input"]>;
  not?: InputMaybe<NestedBoolNullableFilter>;
}

export interface NestedBoolNullableWithAggregatesFilter {
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedBoolNullableFilter>;
  _min?: InputMaybe<NestedBoolNullableFilter>;
  equals?: InputMaybe<Scalars["Boolean"]["input"]>;
  not?: InputMaybe<NestedBoolNullableWithAggregatesFilter>;
}

export interface NestedBoolWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedBoolFilter>;
  _min?: InputMaybe<NestedBoolFilter>;
  equals?: InputMaybe<Scalars["Boolean"]["input"]>;
  not?: InputMaybe<NestedBoolWithAggregatesFilter>;
}

export interface NestedDateTimeFilter {
  equals?: InputMaybe<Scalars["DateTime"]["input"]>;
  gt?: InputMaybe<Scalars["DateTime"]["input"]>;
  gte?: InputMaybe<Scalars["DateTime"]["input"]>;
  in?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
  lt?: InputMaybe<Scalars["DateTime"]["input"]>;
  lte?: InputMaybe<Scalars["DateTime"]["input"]>;
  not?: InputMaybe<NestedDateTimeFilter>;
  notIn?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
}

export interface NestedDateTimeNullableFilter {
  equals?: InputMaybe<Scalars["DateTime"]["input"]>;
  gt?: InputMaybe<Scalars["DateTime"]["input"]>;
  gte?: InputMaybe<Scalars["DateTime"]["input"]>;
  in?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
  lt?: InputMaybe<Scalars["DateTime"]["input"]>;
  lte?: InputMaybe<Scalars["DateTime"]["input"]>;
  not?: InputMaybe<NestedDateTimeNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
}

export interface NestedDateTimeNullableWithAggregatesFilter {
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedDateTimeNullableFilter>;
  _min?: InputMaybe<NestedDateTimeNullableFilter>;
  equals?: InputMaybe<Scalars["DateTime"]["input"]>;
  gt?: InputMaybe<Scalars["DateTime"]["input"]>;
  gte?: InputMaybe<Scalars["DateTime"]["input"]>;
  in?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
  lt?: InputMaybe<Scalars["DateTime"]["input"]>;
  lte?: InputMaybe<Scalars["DateTime"]["input"]>;
  not?: InputMaybe<NestedDateTimeNullableWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
}

export interface NestedDateTimeWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedDateTimeFilter>;
  _min?: InputMaybe<NestedDateTimeFilter>;
  equals?: InputMaybe<Scalars["DateTime"]["input"]>;
  gt?: InputMaybe<Scalars["DateTime"]["input"]>;
  gte?: InputMaybe<Scalars["DateTime"]["input"]>;
  in?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
  lt?: InputMaybe<Scalars["DateTime"]["input"]>;
  lte?: InputMaybe<Scalars["DateTime"]["input"]>;
  not?: InputMaybe<NestedDateTimeWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["DateTime"]["input"]>>;
}

export interface NestedEnumTransferTypeFilter {
  equals?: InputMaybe<TransferType>;
  in?: InputMaybe<TransferType[]>;
  not?: InputMaybe<TransferType>;
  notIn?: InputMaybe<TransferType[]>;
}

export interface NestedEnumTransferTypeWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedEnumTransferTypeFilter>;
  _min?: InputMaybe<NestedEnumTransferTypeFilter>;
  equals?: InputMaybe<TransferType>;
  in?: InputMaybe<TransferType[]>;
  not?: InputMaybe<TransferType>;
  notIn?: InputMaybe<TransferType[]>;
}

export interface NestedFloatFilter {
  equals?: InputMaybe<Scalars["Float"]["input"]>;
  gt?: InputMaybe<Scalars["Float"]["input"]>;
  gte?: InputMaybe<Scalars["Float"]["input"]>;
  in?: InputMaybe<Array<Scalars["Float"]["input"]>>;
  lt?: InputMaybe<Scalars["Float"]["input"]>;
  lte?: InputMaybe<Scalars["Float"]["input"]>;
  not?: InputMaybe<NestedFloatFilter>;
  notIn?: InputMaybe<Array<Scalars["Float"]["input"]>>;
}

export interface NestedFloatNullableFilter {
  equals?: InputMaybe<Scalars["Float"]["input"]>;
  gt?: InputMaybe<Scalars["Float"]["input"]>;
  gte?: InputMaybe<Scalars["Float"]["input"]>;
  in?: InputMaybe<Array<Scalars["Float"]["input"]>>;
  lt?: InputMaybe<Scalars["Float"]["input"]>;
  lte?: InputMaybe<Scalars["Float"]["input"]>;
  not?: InputMaybe<NestedFloatNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["Float"]["input"]>>;
}

export interface NestedFloatWithAggregatesFilter {
  _avg?: InputMaybe<NestedFloatFilter>;
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedFloatFilter>;
  _min?: InputMaybe<NestedFloatFilter>;
  _sum?: InputMaybe<NestedFloatFilter>;
  equals?: InputMaybe<Scalars["Float"]["input"]>;
  gt?: InputMaybe<Scalars["Float"]["input"]>;
  gte?: InputMaybe<Scalars["Float"]["input"]>;
  in?: InputMaybe<Array<Scalars["Float"]["input"]>>;
  lt?: InputMaybe<Scalars["Float"]["input"]>;
  lte?: InputMaybe<Scalars["Float"]["input"]>;
  not?: InputMaybe<NestedFloatWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["Float"]["input"]>>;
}

export interface NestedIntFilter {
  equals?: InputMaybe<Scalars["Int"]["input"]>;
  gt?: InputMaybe<Scalars["Int"]["input"]>;
  gte?: InputMaybe<Scalars["Int"]["input"]>;
  in?: InputMaybe<Array<Scalars["Int"]["input"]>>;
  lt?: InputMaybe<Scalars["Int"]["input"]>;
  lte?: InputMaybe<Scalars["Int"]["input"]>;
  not?: InputMaybe<NestedIntFilter>;
  notIn?: InputMaybe<Array<Scalars["Int"]["input"]>>;
}

export interface NestedIntNullableFilter {
  equals?: InputMaybe<Scalars["Int"]["input"]>;
  gt?: InputMaybe<Scalars["Int"]["input"]>;
  gte?: InputMaybe<Scalars["Int"]["input"]>;
  in?: InputMaybe<Array<Scalars["Int"]["input"]>>;
  lt?: InputMaybe<Scalars["Int"]["input"]>;
  lte?: InputMaybe<Scalars["Int"]["input"]>;
  not?: InputMaybe<NestedIntNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["Int"]["input"]>>;
}

export interface NestedIntNullableWithAggregatesFilter {
  _avg?: InputMaybe<NestedFloatNullableFilter>;
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedIntNullableFilter>;
  _min?: InputMaybe<NestedIntNullableFilter>;
  _sum?: InputMaybe<NestedIntNullableFilter>;
  equals?: InputMaybe<Scalars["Int"]["input"]>;
  gt?: InputMaybe<Scalars["Int"]["input"]>;
  gte?: InputMaybe<Scalars["Int"]["input"]>;
  in?: InputMaybe<Array<Scalars["Int"]["input"]>>;
  lt?: InputMaybe<Scalars["Int"]["input"]>;
  lte?: InputMaybe<Scalars["Int"]["input"]>;
  not?: InputMaybe<NestedIntNullableWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["Int"]["input"]>>;
}

export interface NestedIntWithAggregatesFilter {
  _avg?: InputMaybe<NestedFloatFilter>;
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedIntFilter>;
  _min?: InputMaybe<NestedIntFilter>;
  _sum?: InputMaybe<NestedIntFilter>;
  equals?: InputMaybe<Scalars["Int"]["input"]>;
  gt?: InputMaybe<Scalars["Int"]["input"]>;
  gte?: InputMaybe<Scalars["Int"]["input"]>;
  in?: InputMaybe<Array<Scalars["Int"]["input"]>>;
  lt?: InputMaybe<Scalars["Int"]["input"]>;
  lte?: InputMaybe<Scalars["Int"]["input"]>;
  not?: InputMaybe<NestedIntWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["Int"]["input"]>>;
}

export interface NestedJsonFilter {
  array_contains?: InputMaybe<Scalars["Json"]["input"]>;
  array_ends_with?: InputMaybe<Scalars["Json"]["input"]>;
  array_starts_with?: InputMaybe<Scalars["Json"]["input"]>;
  equals?: InputMaybe<Scalars["Json"]["input"]>;
  gt?: InputMaybe<Scalars["Json"]["input"]>;
  gte?: InputMaybe<Scalars["Json"]["input"]>;
  lt?: InputMaybe<Scalars["Json"]["input"]>;
  lte?: InputMaybe<Scalars["Json"]["input"]>;
  not?: InputMaybe<Scalars["Json"]["input"]>;
  path?: InputMaybe<Array<Scalars["String"]["input"]>>;
  string_contains?: InputMaybe<Scalars["String"]["input"]>;
  string_ends_with?: InputMaybe<Scalars["String"]["input"]>;
  string_starts_with?: InputMaybe<Scalars["String"]["input"]>;
}

export interface NestedStringFilter {
  contains?: InputMaybe<Scalars["String"]["input"]>;
  endsWith?: InputMaybe<Scalars["String"]["input"]>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  not?: InputMaybe<NestedStringFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
  startsWith?: InputMaybe<Scalars["String"]["input"]>;
}

export interface NestedStringNullableFilter {
  contains?: InputMaybe<Scalars["String"]["input"]>;
  endsWith?: InputMaybe<Scalars["String"]["input"]>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  not?: InputMaybe<NestedStringNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
  startsWith?: InputMaybe<Scalars["String"]["input"]>;
}

export interface NestedStringNullableWithAggregatesFilter {
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedStringNullableFilter>;
  _min?: InputMaybe<NestedStringNullableFilter>;
  contains?: InputMaybe<Scalars["String"]["input"]>;
  endsWith?: InputMaybe<Scalars["String"]["input"]>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  not?: InputMaybe<NestedStringNullableWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
  startsWith?: InputMaybe<Scalars["String"]["input"]>;
}

export interface NestedStringWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedStringFilter>;
  _min?: InputMaybe<NestedStringFilter>;
  contains?: InputMaybe<Scalars["String"]["input"]>;
  endsWith?: InputMaybe<Scalars["String"]["input"]>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  not?: InputMaybe<NestedStringWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
  startsWith?: InputMaybe<Scalars["String"]["input"]>;
}

export interface NestedUuidFilter {
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  not?: InputMaybe<NestedUuidFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
}

export interface NestedUuidNullableFilter {
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  not?: InputMaybe<NestedUuidNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
}

export interface NestedUuidNullableWithAggregatesFilter {
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedStringNullableFilter>;
  _min?: InputMaybe<NestedStringNullableFilter>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  not?: InputMaybe<NestedUuidNullableWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
}

export interface NestedUuidWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedStringFilter>;
  _min?: InputMaybe<NestedStringFilter>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  not?: InputMaybe<NestedUuidWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
}

export interface NullableBoolFieldUpdateOperationsInput {
  set?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface NullableDateTimeFieldUpdateOperationsInput {
  set?: InputMaybe<Scalars["DateTime"]["input"]>;
}

export interface NullableIntFieldUpdateOperationsInput {
  decrement?: InputMaybe<Scalars["Int"]["input"]>;
  divide?: InputMaybe<Scalars["Int"]["input"]>;
  increment?: InputMaybe<Scalars["Int"]["input"]>;
  multiply?: InputMaybe<Scalars["Int"]["input"]>;
  set?: InputMaybe<Scalars["Int"]["input"]>;
}

export interface NullableStringFieldUpdateOperationsInput {
  set?: InputMaybe<Scalars["String"]["input"]>;
}

export enum NullsOrder {
  First = "first",
  Last = "last",
}

export interface Pass {
  __typename?: "Pass";
  actualRiseSet?: Maybe<ActualRiseSet>;
  command: Command[];
  groundStation: GroundStation;
  groundStationId: Scalars["Int"]["output"];
  id: Scalars["ID"]["output"];
  memoryAllocated: Scalars["Int"]["output"];
  memoryBudget: Scalars["Int"]["output"];
  nextPass?: Maybe<Pass>;
  predictedRiseSet: PredictedRiseSet;
  predictedRiseSetId: Scalars["String"]["output"];
  previousPass?: Maybe<Pass>;
  previousPassId?: Maybe<Scalars["String"]["output"]>;
  previousPredictedRiseSet: PredictedRiseSet[];
  transfer: Transfer[];
}

export interface PassCommandArgs {
  cursor?: InputMaybe<CommandWhereUniqueInput>;
  distinct?: InputMaybe<CommandScalarFieldEnum[]>;
  orderBy?: InputMaybe<CommandOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<CommandWhereInput>;
}

export interface PassPreviousPredictedRiseSetArgs {
  cursor?: InputMaybe<PredictedRiseSetWhereUniqueInput>;
  distinct?: InputMaybe<PredictedRiseSetScalarFieldEnum[]>;
  orderBy?: InputMaybe<PredictedRiseSetOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface PassTransferArgs {
  cursor?: InputMaybe<TransferWhereUniqueInput>;
  distinct?: InputMaybe<TransferScalarFieldEnum[]>;
  orderBy?: InputMaybe<TransferOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<TransferWhereInput>;
}

export interface PassAvgOrderByAggregateInput {
  groundStationId?: InputMaybe<SortOrder>;
  memoryAllocated?: InputMaybe<SortOrder>;
  memoryBudget?: InputMaybe<SortOrder>;
}

export interface PassCountOrderByAggregateInput {
  groundStationId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  memoryAllocated?: InputMaybe<SortOrder>;
  memoryBudget?: InputMaybe<SortOrder>;
  predictedRiseSetId?: InputMaybe<SortOrder>;
  previousPassId?: InputMaybe<SortOrder>;
}

export interface PassCreateInput {
  actualRiseSet?: InputMaybe<ActualRiseSetCreateNestedOneWithoutPassInput>;
  command?: InputMaybe<CommandCreateNestedManyWithoutPassInput>;
  groundStation: GroundStationCreateNestedOneWithoutPassScheduleInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  nextPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPassInput>;
  predictedRiseSet: PredictedRiseSetCreateNestedOneWithoutCurrentPassInput;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutNextPassInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutPreviousPassInput>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutPassInput>;
}

export interface PassCreateManyGroundStationInput {
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  predictedRiseSetId: Scalars["String"]["input"];
  previousPassId?: InputMaybe<Scalars["String"]["input"]>;
}

export interface PassCreateManyGroundStationInputEnvelope {
  data: PassCreateManyGroundStationInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface PassCreateManyInput {
  groundStationId: Scalars["Int"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  predictedRiseSetId: Scalars["String"]["input"];
  previousPassId?: InputMaybe<Scalars["String"]["input"]>;
}

export interface PassCreateNestedManyWithoutGroundStationInput {
  connect?: InputMaybe<PassWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutGroundStationInput[]>;
  create?: InputMaybe<PassCreateWithoutGroundStationInput[]>;
  createMany?: InputMaybe<PassCreateManyGroundStationInputEnvelope>;
}

export interface PassCreateNestedOneWithoutActualRiseSetInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutActualRiseSetInput>;
  create?: InputMaybe<PassCreateWithoutActualRiseSetInput>;
}

export interface PassCreateNestedOneWithoutCommandInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutCommandInput>;
  create?: InputMaybe<PassCreateWithoutCommandInput>;
}

export interface PassCreateNestedOneWithoutNextPassInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutNextPassInput>;
  create?: InputMaybe<PassCreateWithoutNextPassInput>;
}

export interface PassCreateNestedOneWithoutPredictedRiseSetInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutPredictedRiseSetInput>;
  create?: InputMaybe<PassCreateWithoutPredictedRiseSetInput>;
}

export interface PassCreateNestedOneWithoutPreviousPassInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutPreviousPassInput>;
  create?: InputMaybe<PassCreateWithoutPreviousPassInput>;
}

export interface PassCreateNestedOneWithoutPreviousPredictedRiseSetInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutPreviousPredictedRiseSetInput>;
  create?: InputMaybe<PassCreateWithoutPreviousPredictedRiseSetInput>;
}

export interface PassCreateNestedOneWithoutTransferInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutTransferInput>;
  create?: InputMaybe<PassCreateWithoutTransferInput>;
}

export interface PassCreateOrConnectWithoutActualRiseSetInput {
  create: PassCreateWithoutActualRiseSetInput;
  where: PassWhereUniqueInput;
}

export interface PassCreateOrConnectWithoutCommandInput {
  create: PassCreateWithoutCommandInput;
  where: PassWhereUniqueInput;
}

export interface PassCreateOrConnectWithoutGroundStationInput {
  create: PassCreateWithoutGroundStationInput;
  where: PassWhereUniqueInput;
}

export interface PassCreateOrConnectWithoutNextPassInput {
  create: PassCreateWithoutNextPassInput;
  where: PassWhereUniqueInput;
}

export interface PassCreateOrConnectWithoutPredictedRiseSetInput {
  create: PassCreateWithoutPredictedRiseSetInput;
  where: PassWhereUniqueInput;
}

export interface PassCreateOrConnectWithoutPreviousPassInput {
  create: PassCreateWithoutPreviousPassInput;
  where: PassWhereUniqueInput;
}

export interface PassCreateOrConnectWithoutPreviousPredictedRiseSetInput {
  create: PassCreateWithoutPreviousPredictedRiseSetInput;
  where: PassWhereUniqueInput;
}

export interface PassCreateOrConnectWithoutTransferInput {
  create: PassCreateWithoutTransferInput;
  where: PassWhereUniqueInput;
}

export interface PassCreateWithoutActualRiseSetInput {
  command?: InputMaybe<CommandCreateNestedManyWithoutPassInput>;
  groundStation: GroundStationCreateNestedOneWithoutPassScheduleInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  nextPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPassInput>;
  predictedRiseSet: PredictedRiseSetCreateNestedOneWithoutCurrentPassInput;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutNextPassInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutPreviousPassInput>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutPassInput>;
}

export interface PassCreateWithoutCommandInput {
  actualRiseSet?: InputMaybe<ActualRiseSetCreateNestedOneWithoutPassInput>;
  groundStation: GroundStationCreateNestedOneWithoutPassScheduleInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  nextPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPassInput>;
  predictedRiseSet: PredictedRiseSetCreateNestedOneWithoutCurrentPassInput;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutNextPassInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutPreviousPassInput>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutPassInput>;
}

export interface PassCreateWithoutGroundStationInput {
  actualRiseSet?: InputMaybe<ActualRiseSetCreateNestedOneWithoutPassInput>;
  command?: InputMaybe<CommandCreateNestedManyWithoutPassInput>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  nextPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPassInput>;
  predictedRiseSet: PredictedRiseSetCreateNestedOneWithoutCurrentPassInput;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutNextPassInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutPreviousPassInput>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutPassInput>;
}

export interface PassCreateWithoutNextPassInput {
  actualRiseSet?: InputMaybe<ActualRiseSetCreateNestedOneWithoutPassInput>;
  command?: InputMaybe<CommandCreateNestedManyWithoutPassInput>;
  groundStation: GroundStationCreateNestedOneWithoutPassScheduleInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  predictedRiseSet: PredictedRiseSetCreateNestedOneWithoutCurrentPassInput;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutNextPassInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutPreviousPassInput>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutPassInput>;
}

export interface PassCreateWithoutPredictedRiseSetInput {
  actualRiseSet?: InputMaybe<ActualRiseSetCreateNestedOneWithoutPassInput>;
  command?: InputMaybe<CommandCreateNestedManyWithoutPassInput>;
  groundStation: GroundStationCreateNestedOneWithoutPassScheduleInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  nextPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPassInput>;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutNextPassInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutPreviousPassInput>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutPassInput>;
}

export interface PassCreateWithoutPreviousPassInput {
  actualRiseSet?: InputMaybe<ActualRiseSetCreateNestedOneWithoutPassInput>;
  command?: InputMaybe<CommandCreateNestedManyWithoutPassInput>;
  groundStation: GroundStationCreateNestedOneWithoutPassScheduleInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  nextPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPassInput>;
  predictedRiseSet: PredictedRiseSetCreateNestedOneWithoutCurrentPassInput;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutPreviousPassInput>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutPassInput>;
}

export interface PassCreateWithoutPreviousPredictedRiseSetInput {
  actualRiseSet?: InputMaybe<ActualRiseSetCreateNestedOneWithoutPassInput>;
  command?: InputMaybe<CommandCreateNestedManyWithoutPassInput>;
  groundStation: GroundStationCreateNestedOneWithoutPassScheduleInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  nextPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPassInput>;
  predictedRiseSet: PredictedRiseSetCreateNestedOneWithoutCurrentPassInput;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutNextPassInput>;
  transfer?: InputMaybe<TransferCreateNestedManyWithoutPassInput>;
}

export interface PassCreateWithoutTransferInput {
  actualRiseSet?: InputMaybe<ActualRiseSetCreateNestedOneWithoutPassInput>;
  command?: InputMaybe<CommandCreateNestedManyWithoutPassInput>;
  groundStation: GroundStationCreateNestedOneWithoutPassScheduleInput;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<Scalars["Int"]["input"]>;
  memoryBudget?: InputMaybe<Scalars["Int"]["input"]>;
  nextPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPassInput>;
  predictedRiseSet: PredictedRiseSetCreateNestedOneWithoutCurrentPassInput;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutNextPassInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutPreviousPassInput>;
}

export interface PassListRelationFilter {
  every?: InputMaybe<PassWhereInput>;
  none?: InputMaybe<PassWhereInput>;
  some?: InputMaybe<PassWhereInput>;
}

export interface PassMaxOrderByAggregateInput {
  groundStationId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  memoryAllocated?: InputMaybe<SortOrder>;
  memoryBudget?: InputMaybe<SortOrder>;
  predictedRiseSetId?: InputMaybe<SortOrder>;
  previousPassId?: InputMaybe<SortOrder>;
}

export interface PassMinOrderByAggregateInput {
  groundStationId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  memoryAllocated?: InputMaybe<SortOrder>;
  memoryBudget?: InputMaybe<SortOrder>;
  predictedRiseSetId?: InputMaybe<SortOrder>;
  previousPassId?: InputMaybe<SortOrder>;
}

export interface PassNullableRelationFilter {
  is?: InputMaybe<PassWhereInput>;
  isNot?: InputMaybe<PassWhereInput>;
}

export interface PassOrderByRelationAggregateInput {
  _count?: InputMaybe<SortOrder>;
}

export interface PassOrderByWithAggregationInput {
  _avg?: InputMaybe<PassAvgOrderByAggregateInput>;
  _count?: InputMaybe<PassCountOrderByAggregateInput>;
  _max?: InputMaybe<PassMaxOrderByAggregateInput>;
  _min?: InputMaybe<PassMinOrderByAggregateInput>;
  _sum?: InputMaybe<PassSumOrderByAggregateInput>;
  groundStationId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  memoryAllocated?: InputMaybe<SortOrder>;
  memoryBudget?: InputMaybe<SortOrder>;
  predictedRiseSetId?: InputMaybe<SortOrder>;
  previousPassId?: InputMaybe<SortOrder>;
}

export interface PassOrderByWithRelationInput {
  actualRiseSet?: InputMaybe<ActualRiseSetOrderByWithRelationInput>;
  command?: InputMaybe<CommandOrderByRelationAggregateInput>;
  groundStation?: InputMaybe<GroundStationOrderByWithRelationInput>;
  groundStationId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  memoryAllocated?: InputMaybe<SortOrder>;
  memoryBudget?: InputMaybe<SortOrder>;
  nextPass?: InputMaybe<PassOrderByWithRelationInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetOrderByWithRelationInput>;
  predictedRiseSetId?: InputMaybe<SortOrder>;
  previousPass?: InputMaybe<PassOrderByWithRelationInput>;
  previousPassId?: InputMaybe<SortOrder>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetOrderByRelationAggregateInput>;
  transfer?: InputMaybe<TransferOrderByRelationAggregateInput>;
}

export interface PassRelationFilter {
  is?: InputMaybe<PassWhereInput>;
  isNot?: InputMaybe<PassWhereInput>;
}

export enum PassScalarFieldEnum {
  GroundStationId = "groundStationId",
  Id = "id",
  MemoryAllocated = "memoryAllocated",
  MemoryBudget = "memoryBudget",
  PredictedRiseSetId = "predictedRiseSetId",
  PreviousPassId = "previousPassId",
}

export interface PassScalarWhereInput {
  AND?: InputMaybe<PassScalarWhereInput[]>;
  NOT?: InputMaybe<PassScalarWhereInput[]>;
  OR?: InputMaybe<PassScalarWhereInput[]>;
  groundStationId?: InputMaybe<IntFilter>;
  id?: InputMaybe<UuidFilter>;
  memoryAllocated?: InputMaybe<IntFilter>;
  memoryBudget?: InputMaybe<IntFilter>;
  predictedRiseSetId?: InputMaybe<UuidFilter>;
  previousPassId?: InputMaybe<UuidNullableFilter>;
}

export interface PassScalarWhereWithAggregatesInput {
  AND?: InputMaybe<PassScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<PassScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<PassScalarWhereWithAggregatesInput[]>;
  groundStationId?: InputMaybe<IntWithAggregatesFilter>;
  id?: InputMaybe<UuidWithAggregatesFilter>;
  memoryAllocated?: InputMaybe<IntWithAggregatesFilter>;
  memoryBudget?: InputMaybe<IntWithAggregatesFilter>;
  predictedRiseSetId?: InputMaybe<UuidWithAggregatesFilter>;
  previousPassId?: InputMaybe<UuidNullableWithAggregatesFilter>;
}

export interface PassSchedulingUpdatePassAppendInput {
  groundStationId: Scalars["Int"]["input"];
  id: Scalars["String"]["input"];
  predictedRiseSet: RiseSetInput;
  previousPassId?: InputMaybe<Scalars["String"]["input"]>;
}

export interface PassSchedulingUpdatePassDeleteInput {
  id: Scalars["String"]["input"];
  rise: Scalars["Int"]["input"];
}

export interface PassSchedulingUpdatePassInsertInput {
  groundStationId: Scalars["Int"]["input"];
  id: Scalars["String"]["input"];
  nextPassId: Scalars["String"]["input"];
  predictedRiseSet: RiseSetInput;
}

export interface PassSchedulingUpdatePassUpdateInput {
  id: Scalars["String"]["input"];
  predictedRiseSet: RiseSetInput;
}

export interface PassSumOrderByAggregateInput {
  groundStationId?: InputMaybe<SortOrder>;
  memoryAllocated?: InputMaybe<SortOrder>;
  memoryBudget?: InputMaybe<SortOrder>;
}

export interface PassUpdateInput {
  actualRiseSet?: InputMaybe<ActualRiseSetUpdateOneWithoutPassNestedInput>;
  command?: InputMaybe<CommandUpdateManyWithoutPassNestedInput>;
  groundStation?: InputMaybe<GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  nextPass?: InputMaybe<PassUpdateOneWithoutPreviousPassNestedInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutNextPassNestedInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutPassNestedInput>;
}

export interface PassUpdateManyMutationInput {
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
}

export interface PassUpdateManyWithWhereWithoutGroundStationInput {
  data: PassUpdateManyMutationInput;
  where: PassScalarWhereInput;
}

export interface PassUpdateManyWithoutGroundStationNestedInput {
  connect?: InputMaybe<PassWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutGroundStationInput[]>;
  create?: InputMaybe<PassCreateWithoutGroundStationInput[]>;
  createMany?: InputMaybe<PassCreateManyGroundStationInputEnvelope>;
  delete?: InputMaybe<PassWhereUniqueInput[]>;
  deleteMany?: InputMaybe<PassScalarWhereInput[]>;
  disconnect?: InputMaybe<PassWhereUniqueInput[]>;
  set?: InputMaybe<PassWhereUniqueInput[]>;
  update?: InputMaybe<PassUpdateWithWhereUniqueWithoutGroundStationInput[]>;
  updateMany?: InputMaybe<PassUpdateManyWithWhereWithoutGroundStationInput[]>;
  upsert?: InputMaybe<PassUpsertWithWhereUniqueWithoutGroundStationInput[]>;
}

export interface PassUpdateOneRequiredWithoutActualRiseSetNestedInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutActualRiseSetInput>;
  create?: InputMaybe<PassCreateWithoutActualRiseSetInput>;
  update?: InputMaybe<PassUpdateToOneWithWhereWithoutActualRiseSetInput>;
  upsert?: InputMaybe<PassUpsertWithoutActualRiseSetInput>;
}

export interface PassUpdateOneRequiredWithoutTransferNestedInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutTransferInput>;
  create?: InputMaybe<PassCreateWithoutTransferInput>;
  update?: InputMaybe<PassUpdateToOneWithWhereWithoutTransferInput>;
  upsert?: InputMaybe<PassUpsertWithoutTransferInput>;
}

export interface PassUpdateOneWithoutCommandNestedInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutCommandInput>;
  create?: InputMaybe<PassCreateWithoutCommandInput>;
  delete?: InputMaybe<PassWhereInput>;
  disconnect?: InputMaybe<PassWhereInput>;
  update?: InputMaybe<PassUpdateToOneWithWhereWithoutCommandInput>;
  upsert?: InputMaybe<PassUpsertWithoutCommandInput>;
}

export interface PassUpdateOneWithoutNextPassNestedInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutNextPassInput>;
  create?: InputMaybe<PassCreateWithoutNextPassInput>;
  delete?: InputMaybe<PassWhereInput>;
  disconnect?: InputMaybe<PassWhereInput>;
  update?: InputMaybe<PassUpdateToOneWithWhereWithoutNextPassInput>;
  upsert?: InputMaybe<PassUpsertWithoutNextPassInput>;
}

export interface PassUpdateOneWithoutPredictedRiseSetNestedInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutPredictedRiseSetInput>;
  create?: InputMaybe<PassCreateWithoutPredictedRiseSetInput>;
  delete?: InputMaybe<PassWhereInput>;
  disconnect?: InputMaybe<PassWhereInput>;
  update?: InputMaybe<PassUpdateToOneWithWhereWithoutPredictedRiseSetInput>;
  upsert?: InputMaybe<PassUpsertWithoutPredictedRiseSetInput>;
}

export interface PassUpdateOneWithoutPreviousPassNestedInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutPreviousPassInput>;
  create?: InputMaybe<PassCreateWithoutPreviousPassInput>;
  delete?: InputMaybe<PassWhereInput>;
  disconnect?: InputMaybe<PassWhereInput>;
  update?: InputMaybe<PassUpdateToOneWithWhereWithoutPreviousPassInput>;
  upsert?: InputMaybe<PassUpsertWithoutPreviousPassInput>;
}

export interface PassUpdateOneWithoutPreviousPredictedRiseSetNestedInput {
  connect?: InputMaybe<PassWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PassCreateOrConnectWithoutPreviousPredictedRiseSetInput>;
  create?: InputMaybe<PassCreateWithoutPreviousPredictedRiseSetInput>;
  delete?: InputMaybe<PassWhereInput>;
  disconnect?: InputMaybe<PassWhereInput>;
  update?: InputMaybe<PassUpdateToOneWithWhereWithoutPreviousPredictedRiseSetInput>;
  upsert?: InputMaybe<PassUpsertWithoutPreviousPredictedRiseSetInput>;
}

export interface PassUpdateToOneWithWhereWithoutActualRiseSetInput {
  data: PassUpdateWithoutActualRiseSetInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpdateToOneWithWhereWithoutCommandInput {
  data: PassUpdateWithoutCommandInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpdateToOneWithWhereWithoutNextPassInput {
  data: PassUpdateWithoutNextPassInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpdateToOneWithWhereWithoutPredictedRiseSetInput {
  data: PassUpdateWithoutPredictedRiseSetInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpdateToOneWithWhereWithoutPreviousPassInput {
  data: PassUpdateWithoutPreviousPassInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpdateToOneWithWhereWithoutPreviousPredictedRiseSetInput {
  data: PassUpdateWithoutPreviousPredictedRiseSetInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpdateToOneWithWhereWithoutTransferInput {
  data: PassUpdateWithoutTransferInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpdateWithWhereUniqueWithoutGroundStationInput {
  data: PassUpdateWithoutGroundStationInput;
  where: PassWhereUniqueInput;
}

export interface PassUpdateWithoutActualRiseSetInput {
  command?: InputMaybe<CommandUpdateManyWithoutPassNestedInput>;
  groundStation?: InputMaybe<GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  nextPass?: InputMaybe<PassUpdateOneWithoutPreviousPassNestedInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutNextPassNestedInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutPassNestedInput>;
}

export interface PassUpdateWithoutCommandInput {
  actualRiseSet?: InputMaybe<ActualRiseSetUpdateOneWithoutPassNestedInput>;
  groundStation?: InputMaybe<GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  nextPass?: InputMaybe<PassUpdateOneWithoutPreviousPassNestedInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutNextPassNestedInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutPassNestedInput>;
}

export interface PassUpdateWithoutGroundStationInput {
  actualRiseSet?: InputMaybe<ActualRiseSetUpdateOneWithoutPassNestedInput>;
  command?: InputMaybe<CommandUpdateManyWithoutPassNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  nextPass?: InputMaybe<PassUpdateOneWithoutPreviousPassNestedInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutNextPassNestedInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutPassNestedInput>;
}

export interface PassUpdateWithoutNextPassInput {
  actualRiseSet?: InputMaybe<ActualRiseSetUpdateOneWithoutPassNestedInput>;
  command?: InputMaybe<CommandUpdateManyWithoutPassNestedInput>;
  groundStation?: InputMaybe<GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutNextPassNestedInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutPassNestedInput>;
}

export interface PassUpdateWithoutPredictedRiseSetInput {
  actualRiseSet?: InputMaybe<ActualRiseSetUpdateOneWithoutPassNestedInput>;
  command?: InputMaybe<CommandUpdateManyWithoutPassNestedInput>;
  groundStation?: InputMaybe<GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  nextPass?: InputMaybe<PassUpdateOneWithoutPreviousPassNestedInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutNextPassNestedInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutPassNestedInput>;
}

export interface PassUpdateWithoutPreviousPassInput {
  actualRiseSet?: InputMaybe<ActualRiseSetUpdateOneWithoutPassNestedInput>;
  command?: InputMaybe<CommandUpdateManyWithoutPassNestedInput>;
  groundStation?: InputMaybe<GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  nextPass?: InputMaybe<PassUpdateOneWithoutPreviousPassNestedInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutPassNestedInput>;
}

export interface PassUpdateWithoutPreviousPredictedRiseSetInput {
  actualRiseSet?: InputMaybe<ActualRiseSetUpdateOneWithoutPassNestedInput>;
  command?: InputMaybe<CommandUpdateManyWithoutPassNestedInput>;
  groundStation?: InputMaybe<GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  nextPass?: InputMaybe<PassUpdateOneWithoutPreviousPassNestedInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutNextPassNestedInput>;
  transfer?: InputMaybe<TransferUpdateManyWithoutPassNestedInput>;
}

export interface PassUpdateWithoutTransferInput {
  actualRiseSet?: InputMaybe<ActualRiseSetUpdateOneWithoutPassNestedInput>;
  command?: InputMaybe<CommandUpdateManyWithoutPassNestedInput>;
  groundStation?: InputMaybe<GroundStationUpdateOneRequiredWithoutPassScheduleNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  memoryAllocated?: InputMaybe<IntFieldUpdateOperationsInput>;
  memoryBudget?: InputMaybe<IntFieldUpdateOperationsInput>;
  nextPass?: InputMaybe<PassUpdateOneWithoutPreviousPassNestedInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutNextPassNestedInput>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput>;
}

export interface PassUpsertWithWhereUniqueWithoutGroundStationInput {
  create: PassCreateWithoutGroundStationInput;
  update: PassUpdateWithoutGroundStationInput;
  where: PassWhereUniqueInput;
}

export interface PassUpsertWithoutActualRiseSetInput {
  create: PassCreateWithoutActualRiseSetInput;
  update: PassUpdateWithoutActualRiseSetInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpsertWithoutCommandInput {
  create: PassCreateWithoutCommandInput;
  update: PassUpdateWithoutCommandInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpsertWithoutNextPassInput {
  create: PassCreateWithoutNextPassInput;
  update: PassUpdateWithoutNextPassInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpsertWithoutPredictedRiseSetInput {
  create: PassCreateWithoutPredictedRiseSetInput;
  update: PassUpdateWithoutPredictedRiseSetInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpsertWithoutPreviousPassInput {
  create: PassCreateWithoutPreviousPassInput;
  update: PassUpdateWithoutPreviousPassInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpsertWithoutPreviousPredictedRiseSetInput {
  create: PassCreateWithoutPreviousPredictedRiseSetInput;
  update: PassUpdateWithoutPreviousPredictedRiseSetInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassUpsertWithoutTransferInput {
  create: PassCreateWithoutTransferInput;
  update: PassUpdateWithoutTransferInput;
  where?: InputMaybe<PassWhereInput>;
}

export interface PassWhereInput {
  AND?: InputMaybe<PassWhereInput[]>;
  NOT?: InputMaybe<PassWhereInput[]>;
  OR?: InputMaybe<PassWhereInput[]>;
  actualRiseSet?: InputMaybe<ActualRiseSetWhereInput>;
  command?: InputMaybe<CommandListRelationFilter>;
  groundStation?: InputMaybe<GroundStationWhereInput>;
  groundStationId?: InputMaybe<IntFilter>;
  id?: InputMaybe<UuidFilter>;
  memoryAllocated?: InputMaybe<IntFilter>;
  memoryBudget?: InputMaybe<IntFilter>;
  nextPass?: InputMaybe<PassWhereInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetWhereInput>;
  predictedRiseSetId?: InputMaybe<UuidFilter>;
  previousPass?: InputMaybe<PassWhereInput>;
  previousPassId?: InputMaybe<UuidNullableFilter>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetListRelationFilter>;
  transfer?: InputMaybe<TransferListRelationFilter>;
}

export interface PassWhereUniqueInput {
  AND?: InputMaybe<PassWhereInput[]>;
  NOT?: InputMaybe<PassWhereInput[]>;
  OR?: InputMaybe<PassWhereInput[]>;
  actualRiseSet?: InputMaybe<ActualRiseSetWhereInput>;
  command?: InputMaybe<CommandListRelationFilter>;
  groundStation?: InputMaybe<GroundStationWhereInput>;
  groundStationId?: InputMaybe<IntFilter>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  memoryAllocated?: InputMaybe<IntFilter>;
  memoryBudget?: InputMaybe<IntFilter>;
  nextPass?: InputMaybe<PassWhereInput>;
  predictedRiseSet?: InputMaybe<PredictedRiseSetWhereInput>;
  predictedRiseSetId?: InputMaybe<Scalars["String"]["input"]>;
  previousPass?: InputMaybe<PassWhereInput>;
  previousPassId?: InputMaybe<Scalars["String"]["input"]>;
  previousPredictedRiseSet?: InputMaybe<PredictedRiseSetListRelationFilter>;
  transfer?: InputMaybe<TransferListRelationFilter>;
}

export interface PredictedRiseSet {
  __typename?: "PredictedRiseSet";
  currentPass?: Maybe<Pass>;
  id: Scalars["ID"]["output"];
  previousPass?: Maybe<Pass>;
  previousPassId?: Maybe<Scalars["String"]["output"]>;
  rise: Scalars["Int"]["output"];
  set: Scalars["Int"]["output"];
  stateElement: StateElement;
  stateElementId: Scalars["String"]["output"];
}

export interface PredictedRiseSetAvgOrderByAggregateInput {
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export interface PredictedRiseSetCountOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  previousPassId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
  stateElementId?: InputMaybe<SortOrder>;
}

export interface PredictedRiseSetCreateInput {
  currentPass?: InputMaybe<PassCreateNestedOneWithoutPredictedRiseSetInput>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPredictedRiseSetInput>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
  stateElement: StateElementCreateNestedOneWithoutRiseSetInput;
}

export interface PredictedRiseSetCreateManyInput {
  id?: InputMaybe<Scalars["String"]["input"]>;
  previousPassId?: InputMaybe<Scalars["String"]["input"]>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
  stateElementId: Scalars["String"]["input"];
}

export interface PredictedRiseSetCreateManyPreviousPassInput {
  id?: InputMaybe<Scalars["String"]["input"]>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
  stateElementId: Scalars["String"]["input"];
}

export interface PredictedRiseSetCreateManyPreviousPassInputEnvelope {
  data: PredictedRiseSetCreateManyPreviousPassInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface PredictedRiseSetCreateManyStateElementInput {
  id?: InputMaybe<Scalars["String"]["input"]>;
  previousPassId?: InputMaybe<Scalars["String"]["input"]>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
}

export interface PredictedRiseSetCreateManyStateElementInputEnvelope {
  data: PredictedRiseSetCreateManyStateElementInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface PredictedRiseSetCreateNestedManyWithoutPreviousPassInput {
  connect?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<
    PredictedRiseSetCreateOrConnectWithoutPreviousPassInput[]
  >;
  create?: InputMaybe<PredictedRiseSetCreateWithoutPreviousPassInput[]>;
  createMany?: InputMaybe<PredictedRiseSetCreateManyPreviousPassInputEnvelope>;
}

export interface PredictedRiseSetCreateNestedManyWithoutStateElementInput {
  connect?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<
    PredictedRiseSetCreateOrConnectWithoutStateElementInput[]
  >;
  create?: InputMaybe<PredictedRiseSetCreateWithoutStateElementInput[]>;
  createMany?: InputMaybe<PredictedRiseSetCreateManyStateElementInputEnvelope>;
}

export interface PredictedRiseSetCreateNestedOneWithoutCurrentPassInput {
  connect?: InputMaybe<PredictedRiseSetWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PredictedRiseSetCreateOrConnectWithoutCurrentPassInput>;
  create?: InputMaybe<PredictedRiseSetCreateWithoutCurrentPassInput>;
}

export interface PredictedRiseSetCreateOrConnectWithoutCurrentPassInput {
  create: PredictedRiseSetCreateWithoutCurrentPassInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface PredictedRiseSetCreateOrConnectWithoutPreviousPassInput {
  create: PredictedRiseSetCreateWithoutPreviousPassInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface PredictedRiseSetCreateOrConnectWithoutStateElementInput {
  create: PredictedRiseSetCreateWithoutStateElementInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface PredictedRiseSetCreateWithoutCurrentPassInput {
  id?: InputMaybe<Scalars["String"]["input"]>;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPredictedRiseSetInput>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
  stateElement: StateElementCreateNestedOneWithoutRiseSetInput;
}

export interface PredictedRiseSetCreateWithoutPreviousPassInput {
  currentPass?: InputMaybe<PassCreateNestedOneWithoutPredictedRiseSetInput>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
  stateElement: StateElementCreateNestedOneWithoutRiseSetInput;
}

export interface PredictedRiseSetCreateWithoutStateElementInput {
  currentPass?: InputMaybe<PassCreateNestedOneWithoutPredictedRiseSetInput>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  previousPass?: InputMaybe<PassCreateNestedOneWithoutPreviousPredictedRiseSetInput>;
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
}

export interface PredictedRiseSetListRelationFilter {
  every?: InputMaybe<PredictedRiseSetWhereInput>;
  none?: InputMaybe<PredictedRiseSetWhereInput>;
  some?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface PredictedRiseSetMaxOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  previousPassId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
  stateElementId?: InputMaybe<SortOrder>;
}

export interface PredictedRiseSetMinOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  previousPassId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
  stateElementId?: InputMaybe<SortOrder>;
}

export interface PredictedRiseSetOrderByRelationAggregateInput {
  _count?: InputMaybe<SortOrder>;
}

export interface PredictedRiseSetOrderByWithAggregationInput {
  _avg?: InputMaybe<PredictedRiseSetAvgOrderByAggregateInput>;
  _count?: InputMaybe<PredictedRiseSetCountOrderByAggregateInput>;
  _max?: InputMaybe<PredictedRiseSetMaxOrderByAggregateInput>;
  _min?: InputMaybe<PredictedRiseSetMinOrderByAggregateInput>;
  _sum?: InputMaybe<PredictedRiseSetSumOrderByAggregateInput>;
  id?: InputMaybe<SortOrder>;
  previousPassId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
  stateElementId?: InputMaybe<SortOrder>;
}

export interface PredictedRiseSetOrderByWithRelationInput {
  currentPass?: InputMaybe<PassOrderByWithRelationInput>;
  id?: InputMaybe<SortOrder>;
  previousPass?: InputMaybe<PassOrderByWithRelationInput>;
  previousPassId?: InputMaybe<SortOrder>;
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
  stateElement?: InputMaybe<StateElementOrderByWithRelationInput>;
  stateElementId?: InputMaybe<SortOrder>;
}

export interface PredictedRiseSetRelationFilter {
  is?: InputMaybe<PredictedRiseSetWhereInput>;
  isNot?: InputMaybe<PredictedRiseSetWhereInput>;
}

export enum PredictedRiseSetScalarFieldEnum {
  Id = "id",
  PreviousPassId = "previousPassId",
  Rise = "rise",
  Set = "set",
  StateElementId = "stateElementId",
}

export interface PredictedRiseSetScalarWhereInput {
  AND?: InputMaybe<PredictedRiseSetScalarWhereInput[]>;
  NOT?: InputMaybe<PredictedRiseSetScalarWhereInput[]>;
  OR?: InputMaybe<PredictedRiseSetScalarWhereInput[]>;
  id?: InputMaybe<UuidFilter>;
  previousPassId?: InputMaybe<UuidNullableFilter>;
  rise?: InputMaybe<IntFilter>;
  set?: InputMaybe<IntFilter>;
  stateElementId?: InputMaybe<UuidFilter>;
}

export interface PredictedRiseSetScalarWhereWithAggregatesInput {
  AND?: InputMaybe<PredictedRiseSetScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<PredictedRiseSetScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<PredictedRiseSetScalarWhereWithAggregatesInput[]>;
  id?: InputMaybe<UuidWithAggregatesFilter>;
  previousPassId?: InputMaybe<UuidNullableWithAggregatesFilter>;
  rise?: InputMaybe<IntWithAggregatesFilter>;
  set?: InputMaybe<IntWithAggregatesFilter>;
  stateElementId?: InputMaybe<UuidWithAggregatesFilter>;
}

export interface PredictedRiseSetSumOrderByAggregateInput {
  rise?: InputMaybe<SortOrder>;
  set?: InputMaybe<SortOrder>;
}

export interface PredictedRiseSetUpdateInput {
  currentPass?: InputMaybe<PassUpdateOneWithoutPredictedRiseSetNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutPreviousPredictedRiseSetNestedInput>;
  rise?: InputMaybe<IntFieldUpdateOperationsInput>;
  set?: InputMaybe<IntFieldUpdateOperationsInput>;
  stateElement?: InputMaybe<StateElementUpdateOneRequiredWithoutRiseSetNestedInput>;
}

export interface PredictedRiseSetUpdateManyMutationInput {
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  rise?: InputMaybe<IntFieldUpdateOperationsInput>;
  set?: InputMaybe<IntFieldUpdateOperationsInput>;
}

export interface PredictedRiseSetUpdateManyWithWhereWithoutPreviousPassInput {
  data: PredictedRiseSetUpdateManyMutationInput;
  where: PredictedRiseSetScalarWhereInput;
}

export interface PredictedRiseSetUpdateManyWithWhereWithoutStateElementInput {
  data: PredictedRiseSetUpdateManyMutationInput;
  where: PredictedRiseSetScalarWhereInput;
}

export interface PredictedRiseSetUpdateManyWithoutPreviousPassNestedInput {
  connect?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<
    PredictedRiseSetCreateOrConnectWithoutPreviousPassInput[]
  >;
  create?: InputMaybe<PredictedRiseSetCreateWithoutPreviousPassInput[]>;
  createMany?: InputMaybe<PredictedRiseSetCreateManyPreviousPassInputEnvelope>;
  delete?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  deleteMany?: InputMaybe<PredictedRiseSetScalarWhereInput[]>;
  disconnect?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  set?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  update?: InputMaybe<
    PredictedRiseSetUpdateWithWhereUniqueWithoutPreviousPassInput[]
  >;
  updateMany?: InputMaybe<
    PredictedRiseSetUpdateManyWithWhereWithoutPreviousPassInput[]
  >;
  upsert?: InputMaybe<
    PredictedRiseSetUpsertWithWhereUniqueWithoutPreviousPassInput[]
  >;
}

export interface PredictedRiseSetUpdateManyWithoutStateElementNestedInput {
  connect?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<
    PredictedRiseSetCreateOrConnectWithoutStateElementInput[]
  >;
  create?: InputMaybe<PredictedRiseSetCreateWithoutStateElementInput[]>;
  createMany?: InputMaybe<PredictedRiseSetCreateManyStateElementInputEnvelope>;
  delete?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  deleteMany?: InputMaybe<PredictedRiseSetScalarWhereInput[]>;
  disconnect?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  set?: InputMaybe<PredictedRiseSetWhereUniqueInput[]>;
  update?: InputMaybe<
    PredictedRiseSetUpdateWithWhereUniqueWithoutStateElementInput[]
  >;
  updateMany?: InputMaybe<
    PredictedRiseSetUpdateManyWithWhereWithoutStateElementInput[]
  >;
  upsert?: InputMaybe<
    PredictedRiseSetUpsertWithWhereUniqueWithoutStateElementInput[]
  >;
}

export interface PredictedRiseSetUpdateOneRequiredWithoutCurrentPassNestedInput {
  connect?: InputMaybe<PredictedRiseSetWhereUniqueInput>;
  connectOrCreate?: InputMaybe<PredictedRiseSetCreateOrConnectWithoutCurrentPassInput>;
  create?: InputMaybe<PredictedRiseSetCreateWithoutCurrentPassInput>;
  update?: InputMaybe<PredictedRiseSetUpdateToOneWithWhereWithoutCurrentPassInput>;
  upsert?: InputMaybe<PredictedRiseSetUpsertWithoutCurrentPassInput>;
}

export interface PredictedRiseSetUpdateToOneWithWhereWithoutCurrentPassInput {
  data: PredictedRiseSetUpdateWithoutCurrentPassInput;
  where?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface PredictedRiseSetUpdateWithWhereUniqueWithoutPreviousPassInput {
  data: PredictedRiseSetUpdateWithoutPreviousPassInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface PredictedRiseSetUpdateWithWhereUniqueWithoutStateElementInput {
  data: PredictedRiseSetUpdateWithoutStateElementInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface PredictedRiseSetUpdateWithoutCurrentPassInput {
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutPreviousPredictedRiseSetNestedInput>;
  rise?: InputMaybe<IntFieldUpdateOperationsInput>;
  set?: InputMaybe<IntFieldUpdateOperationsInput>;
  stateElement?: InputMaybe<StateElementUpdateOneRequiredWithoutRiseSetNestedInput>;
}

export interface PredictedRiseSetUpdateWithoutPreviousPassInput {
  currentPass?: InputMaybe<PassUpdateOneWithoutPredictedRiseSetNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  rise?: InputMaybe<IntFieldUpdateOperationsInput>;
  set?: InputMaybe<IntFieldUpdateOperationsInput>;
  stateElement?: InputMaybe<StateElementUpdateOneRequiredWithoutRiseSetNestedInput>;
}

export interface PredictedRiseSetUpdateWithoutStateElementInput {
  currentPass?: InputMaybe<PassUpdateOneWithoutPredictedRiseSetNestedInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  previousPass?: InputMaybe<PassUpdateOneWithoutPreviousPredictedRiseSetNestedInput>;
  rise?: InputMaybe<IntFieldUpdateOperationsInput>;
  set?: InputMaybe<IntFieldUpdateOperationsInput>;
}

export interface PredictedRiseSetUpsertWithWhereUniqueWithoutPreviousPassInput {
  create: PredictedRiseSetCreateWithoutPreviousPassInput;
  update: PredictedRiseSetUpdateWithoutPreviousPassInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface PredictedRiseSetUpsertWithWhereUniqueWithoutStateElementInput {
  create: PredictedRiseSetCreateWithoutStateElementInput;
  update: PredictedRiseSetUpdateWithoutStateElementInput;
  where: PredictedRiseSetWhereUniqueInput;
}

export interface PredictedRiseSetUpsertWithoutCurrentPassInput {
  create: PredictedRiseSetCreateWithoutCurrentPassInput;
  update: PredictedRiseSetUpdateWithoutCurrentPassInput;
  where?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface PredictedRiseSetWhereInput {
  AND?: InputMaybe<PredictedRiseSetWhereInput[]>;
  NOT?: InputMaybe<PredictedRiseSetWhereInput[]>;
  OR?: InputMaybe<PredictedRiseSetWhereInput[]>;
  currentPass?: InputMaybe<PassWhereInput>;
  id?: InputMaybe<UuidFilter>;
  previousPass?: InputMaybe<PassWhereInput>;
  previousPassId?: InputMaybe<UuidNullableFilter>;
  rise?: InputMaybe<IntFilter>;
  set?: InputMaybe<IntFilter>;
  stateElement?: InputMaybe<StateElementWhereInput>;
  stateElementId?: InputMaybe<UuidFilter>;
}

export interface PredictedRiseSetWhereUniqueInput {
  AND?: InputMaybe<PredictedRiseSetWhereInput[]>;
  NOT?: InputMaybe<PredictedRiseSetWhereInput[]>;
  OR?: InputMaybe<PredictedRiseSetWhereInput[]>;
  currentPass?: InputMaybe<PassWhereInput>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  previousPass?: InputMaybe<PassWhereInput>;
  previousPassId?: InputMaybe<UuidNullableFilter>;
  rise?: InputMaybe<IntFilter>;
  set?: InputMaybe<IntFilter>;
  stateElement?: InputMaybe<StateElementWhereInput>;
  stateElementId?: InputMaybe<UuidFilter>;
}

export interface Query {
  __typename?: "Query";
  countActualRiseSet: Scalars["Int"]["output"];
  countAdmin: Scalars["Int"]["output"];
  countCommand: Scalars["Int"]["output"];
  countGroundStation: Scalars["Int"]["output"];
  countLog: Scalars["Int"]["output"];
  countPass: Scalars["Int"]["output"];
  countPredictedRiseSet: Scalars["Int"]["output"];
  countResult: Scalars["Int"]["output"];
  countStateElement: Scalars["Int"]["output"];
  countTransfer: Scalars["Int"]["output"];
  countUser: Scalars["Int"]["output"];
  findFirstActualRiseSet?: Maybe<ActualRiseSet>;
  findFirstAdmin?: Maybe<Admin>;
  findFirstCommand?: Maybe<Command>;
  findFirstGroundStation?: Maybe<GroundStation>;
  findFirstLog?: Maybe<Log>;
  findFirstPass?: Maybe<Pass>;
  findFirstPredictedRiseSet?: Maybe<PredictedRiseSet>;
  findFirstResult?: Maybe<Result>;
  findFirstStateElement?: Maybe<StateElement>;
  findFirstTransfer?: Maybe<Transfer>;
  findFirstUser?: Maybe<User>;
  findManyActualRiseSet: ActualRiseSet[];
  findManyAdmin: Admin[];
  findManyCommand: Command[];
  findManyGroundStation: GroundStation[];
  findManyLog: Log[];
  findManyPass: Pass[];
  findManyPredictedRiseSet: PredictedRiseSet[];
  findManyResult: Result[];
  findManyStateElement: StateElement[];
  findManyTransfer: Transfer[];
  findManyUser: User[];
  findUniqueActualRiseSet?: Maybe<ActualRiseSet>;
  findUniqueAdmin?: Maybe<Admin>;
  findUniqueCommand?: Maybe<Command>;
  findUniqueGroundStation?: Maybe<GroundStation>;
  findUniqueLog?: Maybe<Log>;
  findUniquePass?: Maybe<Pass>;
  findUniquePredictedRiseSet?: Maybe<PredictedRiseSet>;
  findUniqueResult?: Maybe<Result>;
  findUniqueStateElement?: Maybe<StateElement>;
  findUniqueTransfer?: Maybe<Transfer>;
  findUniqueUser?: Maybe<User>;
}

export interface QueryCountActualRiseSetArgs {
  cursor?: InputMaybe<ActualRiseSetWhereUniqueInput>;
  distinct?: InputMaybe<ActualRiseSetScalarFieldEnum[]>;
  orderBy?: InputMaybe<ActualRiseSetOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<ActualRiseSetWhereInput>;
}

export interface QueryCountAdminArgs {
  cursor?: InputMaybe<AdminWhereUniqueInput>;
  distinct?: InputMaybe<AdminScalarFieldEnum[]>;
  orderBy?: InputMaybe<AdminOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<AdminWhereInput>;
}

export interface QueryCountCommandArgs {
  cursor?: InputMaybe<CommandWhereUniqueInput>;
  distinct?: InputMaybe<CommandScalarFieldEnum[]>;
  orderBy?: InputMaybe<CommandOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<CommandWhereInput>;
}

export interface QueryCountGroundStationArgs {
  cursor?: InputMaybe<GroundStationWhereUniqueInput>;
  distinct?: InputMaybe<GroundStationScalarFieldEnum[]>;
  orderBy?: InputMaybe<GroundStationOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<GroundStationWhereInput>;
}

export interface QueryCountLogArgs {
  cursor?: InputMaybe<LogWhereUniqueInput>;
  distinct?: InputMaybe<LogScalarFieldEnum[]>;
  orderBy?: InputMaybe<LogOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<LogWhereInput>;
}

export interface QueryCountPassArgs {
  cursor?: InputMaybe<PassWhereUniqueInput>;
  distinct?: InputMaybe<PassScalarFieldEnum[]>;
  orderBy?: InputMaybe<PassOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PassWhereInput>;
}

export interface QueryCountPredictedRiseSetArgs {
  cursor?: InputMaybe<PredictedRiseSetWhereUniqueInput>;
  distinct?: InputMaybe<PredictedRiseSetScalarFieldEnum[]>;
  orderBy?: InputMaybe<PredictedRiseSetOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface QueryCountResultArgs {
  cursor?: InputMaybe<ResultWhereUniqueInput>;
  distinct?: InputMaybe<ResultScalarFieldEnum[]>;
  orderBy?: InputMaybe<ResultOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<ResultWhereInput>;
}

export interface QueryCountStateElementArgs {
  cursor?: InputMaybe<StateElementWhereUniqueInput>;
  distinct?: InputMaybe<StateElementScalarFieldEnum[]>;
  orderBy?: InputMaybe<StateElementOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<StateElementWhereInput>;
}

export interface QueryCountTransferArgs {
  cursor?: InputMaybe<TransferWhereUniqueInput>;
  distinct?: InputMaybe<TransferScalarFieldEnum[]>;
  orderBy?: InputMaybe<TransferOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<TransferWhereInput>;
}

export interface QueryCountUserArgs {
  cursor?: InputMaybe<UserWhereUniqueInput>;
  distinct?: InputMaybe<UserScalarFieldEnum[]>;
  orderBy?: InputMaybe<UserOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<UserWhereInput>;
}

export interface QueryFindFirstActualRiseSetArgs {
  cursor?: InputMaybe<ActualRiseSetWhereUniqueInput>;
  distinct?: InputMaybe<ActualRiseSetScalarFieldEnum[]>;
  orderBy?: InputMaybe<ActualRiseSetOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<ActualRiseSetWhereInput>;
}

export interface QueryFindFirstAdminArgs {
  cursor?: InputMaybe<AdminWhereUniqueInput>;
  distinct?: InputMaybe<AdminScalarFieldEnum[]>;
  orderBy?: InputMaybe<AdminOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<AdminWhereInput>;
}

export interface QueryFindFirstCommandArgs {
  cursor?: InputMaybe<CommandWhereUniqueInput>;
  distinct?: InputMaybe<CommandScalarFieldEnum[]>;
  orderBy?: InputMaybe<CommandOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<CommandWhereInput>;
}

export interface QueryFindFirstGroundStationArgs {
  cursor?: InputMaybe<GroundStationWhereUniqueInput>;
  distinct?: InputMaybe<GroundStationScalarFieldEnum[]>;
  orderBy?: InputMaybe<GroundStationOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<GroundStationWhereInput>;
}

export interface QueryFindFirstLogArgs {
  cursor?: InputMaybe<LogWhereUniqueInput>;
  distinct?: InputMaybe<LogScalarFieldEnum[]>;
  orderBy?: InputMaybe<LogOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<LogWhereInput>;
}

export interface QueryFindFirstPassArgs {
  cursor?: InputMaybe<PassWhereUniqueInput>;
  distinct?: InputMaybe<PassScalarFieldEnum[]>;
  orderBy?: InputMaybe<PassOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PassWhereInput>;
}

export interface QueryFindFirstPredictedRiseSetArgs {
  cursor?: InputMaybe<PredictedRiseSetWhereUniqueInput>;
  distinct?: InputMaybe<PredictedRiseSetScalarFieldEnum[]>;
  orderBy?: InputMaybe<PredictedRiseSetOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface QueryFindFirstResultArgs {
  cursor?: InputMaybe<ResultWhereUniqueInput>;
  distinct?: InputMaybe<ResultScalarFieldEnum[]>;
  orderBy?: InputMaybe<ResultOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<ResultWhereInput>;
}

export interface QueryFindFirstStateElementArgs {
  cursor?: InputMaybe<StateElementWhereUniqueInput>;
  distinct?: InputMaybe<StateElementScalarFieldEnum[]>;
  orderBy?: InputMaybe<StateElementOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<StateElementWhereInput>;
}

export interface QueryFindFirstTransferArgs {
  cursor?: InputMaybe<TransferWhereUniqueInput>;
  distinct?: InputMaybe<TransferScalarFieldEnum[]>;
  orderBy?: InputMaybe<TransferOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<TransferWhereInput>;
}

export interface QueryFindFirstUserArgs {
  cursor?: InputMaybe<UserWhereUniqueInput>;
  distinct?: InputMaybe<UserScalarFieldEnum[]>;
  orderBy?: InputMaybe<UserOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<UserWhereInput>;
}

export interface QueryFindManyActualRiseSetArgs {
  cursor?: InputMaybe<ActualRiseSetWhereUniqueInput>;
  distinct?: InputMaybe<ActualRiseSetScalarFieldEnum[]>;
  orderBy?: InputMaybe<ActualRiseSetOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<ActualRiseSetWhereInput>;
}

export interface QueryFindManyAdminArgs {
  cursor?: InputMaybe<AdminWhereUniqueInput>;
  distinct?: InputMaybe<AdminScalarFieldEnum[]>;
  orderBy?: InputMaybe<AdminOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<AdminWhereInput>;
}

export interface QueryFindManyCommandArgs {
  cursor?: InputMaybe<CommandWhereUniqueInput>;
  distinct?: InputMaybe<CommandScalarFieldEnum[]>;
  orderBy?: InputMaybe<CommandOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<CommandWhereInput>;
}

export interface QueryFindManyGroundStationArgs {
  cursor?: InputMaybe<GroundStationWhereUniqueInput>;
  distinct?: InputMaybe<GroundStationScalarFieldEnum[]>;
  orderBy?: InputMaybe<GroundStationOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<GroundStationWhereInput>;
}

export interface QueryFindManyLogArgs {
  cursor?: InputMaybe<LogWhereUniqueInput>;
  distinct?: InputMaybe<LogScalarFieldEnum[]>;
  orderBy?: InputMaybe<LogOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<LogWhereInput>;
}

export interface QueryFindManyPassArgs {
  cursor?: InputMaybe<PassWhereUniqueInput>;
  distinct?: InputMaybe<PassScalarFieldEnum[]>;
  orderBy?: InputMaybe<PassOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PassWhereInput>;
}

export interface QueryFindManyPredictedRiseSetArgs {
  cursor?: InputMaybe<PredictedRiseSetWhereUniqueInput>;
  distinct?: InputMaybe<PredictedRiseSetScalarFieldEnum[]>;
  orderBy?: InputMaybe<PredictedRiseSetOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface QueryFindManyResultArgs {
  cursor?: InputMaybe<ResultWhereUniqueInput>;
  distinct?: InputMaybe<ResultScalarFieldEnum[]>;
  orderBy?: InputMaybe<ResultOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<ResultWhereInput>;
}

export interface QueryFindManyStateElementArgs {
  cursor?: InputMaybe<StateElementWhereUniqueInput>;
  distinct?: InputMaybe<StateElementScalarFieldEnum[]>;
  orderBy?: InputMaybe<StateElementOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<StateElementWhereInput>;
}

export interface QueryFindManyTransferArgs {
  cursor?: InputMaybe<TransferWhereUniqueInput>;
  distinct?: InputMaybe<TransferScalarFieldEnum[]>;
  orderBy?: InputMaybe<TransferOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<TransferWhereInput>;
}

export interface QueryFindManyUserArgs {
  cursor?: InputMaybe<UserWhereUniqueInput>;
  distinct?: InputMaybe<UserScalarFieldEnum[]>;
  orderBy?: InputMaybe<UserOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<UserWhereInput>;
}

export interface QueryFindUniqueActualRiseSetArgs {
  where: ActualRiseSetWhereUniqueInput;
}

export interface QueryFindUniqueAdminArgs {
  where: AdminWhereUniqueInput;
}

export interface QueryFindUniqueCommandArgs {
  where: CommandWhereUniqueInput;
}

export interface QueryFindUniqueGroundStationArgs {
  where: GroundStationWhereUniqueInput;
}

export interface QueryFindUniqueLogArgs {
  where: LogWhereUniqueInput;
}

export interface QueryFindUniquePassArgs {
  where: PassWhereUniqueInput;
}

export interface QueryFindUniquePredictedRiseSetArgs {
  where: PredictedRiseSetWhereUniqueInput;
}

export interface QueryFindUniqueResultArgs {
  where: ResultWhereUniqueInput;
}

export interface QueryFindUniqueStateElementArgs {
  where: StateElementWhereUniqueInput;
}

export interface QueryFindUniqueTransferArgs {
  where: TransferWhereUniqueInput;
}

export interface QueryFindUniqueUserArgs {
  where: UserWhereUniqueInput;
}

export enum QueryMode {
  Default = "default",
  Insensitive = "insensitive",
}

export interface Result {
  __typename?: "Result";
  command: Command;
  completed: Scalars["Boolean"]["output"];
  id: Scalars["ID"]["output"];
  result: Scalars["String"]["output"];
}

export interface ResultCountOrderByAggregateInput {
  completed?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  result?: InputMaybe<SortOrder>;
}

export interface ResultCreateInput {
  command?: InputMaybe<CommandCreateNestedOneWithoutResultInput>;
  completed?: InputMaybe<Scalars["Boolean"]["input"]>;
  result: Scalars["String"]["input"];
}

export interface ResultCreateManyInput {
  completed?: InputMaybe<Scalars["Boolean"]["input"]>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  result: Scalars["String"]["input"];
}

export interface ResultCreateNestedOneWithoutCommandInput {
  connect?: InputMaybe<ResultWhereUniqueInput>;
  connectOrCreate?: InputMaybe<ResultCreateOrConnectWithoutCommandInput>;
  create?: InputMaybe<ResultCreateWithoutCommandInput>;
}

export interface ResultCreateOrConnectWithoutCommandInput {
  create: ResultCreateWithoutCommandInput;
  where: ResultWhereUniqueInput;
}

export interface ResultCreateWithoutCommandInput {
  completed?: InputMaybe<Scalars["Boolean"]["input"]>;
  result: Scalars["String"]["input"];
}

export interface ResultMaxOrderByAggregateInput {
  completed?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  result?: InputMaybe<SortOrder>;
}

export interface ResultMinOrderByAggregateInput {
  completed?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  result?: InputMaybe<SortOrder>;
}

export interface ResultNullableRelationFilter {
  is?: InputMaybe<ResultWhereInput>;
  isNot?: InputMaybe<ResultWhereInput>;
}

export interface ResultOrderByWithAggregationInput {
  _count?: InputMaybe<ResultCountOrderByAggregateInput>;
  _max?: InputMaybe<ResultMaxOrderByAggregateInput>;
  _min?: InputMaybe<ResultMinOrderByAggregateInput>;
  completed?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  result?: InputMaybe<SortOrder>;
}

export interface ResultOrderByWithRelationInput {
  command?: InputMaybe<CommandOrderByWithRelationInput>;
  completed?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  result?: InputMaybe<SortOrder>;
}

export enum ResultScalarFieldEnum {
  Completed = "completed",
  Id = "id",
  Result = "result",
}

export interface ResultScalarWhereWithAggregatesInput {
  AND?: InputMaybe<ResultScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<ResultScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<ResultScalarWhereWithAggregatesInput[]>;
  completed?: InputMaybe<BoolWithAggregatesFilter>;
  id?: InputMaybe<UuidWithAggregatesFilter>;
  result?: InputMaybe<StringWithAggregatesFilter>;
}

export interface ResultUpdateInput {
  command?: InputMaybe<CommandUpdateOneRequiredWithoutResultNestedInput>;
  completed?: InputMaybe<BoolFieldUpdateOperationsInput>;
  result?: InputMaybe<StringFieldUpdateOperationsInput>;
}

export interface ResultUpdateManyMutationInput {
  completed?: InputMaybe<BoolFieldUpdateOperationsInput>;
  result?: InputMaybe<StringFieldUpdateOperationsInput>;
}

export interface ResultUpdateOneWithoutCommandNestedInput {
  connect?: InputMaybe<ResultWhereUniqueInput>;
  connectOrCreate?: InputMaybe<ResultCreateOrConnectWithoutCommandInput>;
  create?: InputMaybe<ResultCreateWithoutCommandInput>;
  delete?: InputMaybe<ResultWhereInput>;
  disconnect?: InputMaybe<ResultWhereInput>;
  update?: InputMaybe<ResultUpdateToOneWithWhereWithoutCommandInput>;
  upsert?: InputMaybe<ResultUpsertWithoutCommandInput>;
}

export interface ResultUpdateToOneWithWhereWithoutCommandInput {
  data: ResultUpdateWithoutCommandInput;
  where?: InputMaybe<ResultWhereInput>;
}

export interface ResultUpdateWithoutCommandInput {
  completed?: InputMaybe<BoolFieldUpdateOperationsInput>;
  result?: InputMaybe<StringFieldUpdateOperationsInput>;
}

export interface ResultUpsertWithoutCommandInput {
  create: ResultCreateWithoutCommandInput;
  update: ResultUpdateWithoutCommandInput;
  where?: InputMaybe<ResultWhereInput>;
}

export interface ResultWhereInput {
  AND?: InputMaybe<ResultWhereInput[]>;
  NOT?: InputMaybe<ResultWhereInput[]>;
  OR?: InputMaybe<ResultWhereInput[]>;
  command?: InputMaybe<CommandWhereInput>;
  completed?: InputMaybe<BoolFilter>;
  id?: InputMaybe<UuidFilter>;
  result?: InputMaybe<StringFilter>;
}

export interface ResultWhereUniqueInput {
  AND?: InputMaybe<ResultWhereInput[]>;
  NOT?: InputMaybe<ResultWhereInput[]>;
  OR?: InputMaybe<ResultWhereInput[]>;
  command?: InputMaybe<CommandWhereInput>;
  completed?: InputMaybe<BoolFilter>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  result?: InputMaybe<StringFilter>;
}

export interface RiseSetInput {
  rise: Scalars["Int"]["input"];
  set: Scalars["Int"]["input"];
  stateId: Scalars["String"]["input"];
}

export enum SortOrder {
  Asc = "asc",
  Desc = "desc",
}

export interface StateElement {
  __typename?: "StateElement";
  RiseSet: PredictedRiseSet[];
  epoch: Scalars["BigInt"]["output"];
  id: Scalars["ID"]["output"];
  stateElement: Scalars["Json"]["output"];
  tle?: Maybe<Scalars["String"]["output"]>;
}

export interface StateElementRiseSetArgs {
  cursor?: InputMaybe<PredictedRiseSetWhereUniqueInput>;
  distinct?: InputMaybe<PredictedRiseSetScalarFieldEnum[]>;
  orderBy?: InputMaybe<PredictedRiseSetOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<PredictedRiseSetWhereInput>;
}

export interface StateElementAvgOrderByAggregateInput {
  epoch?: InputMaybe<SortOrder>;
}

export interface StateElementCountOrderByAggregateInput {
  epoch?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  stateElement?: InputMaybe<SortOrder>;
  tle?: InputMaybe<SortOrder>;
}

export interface StateElementCreateInput {
  RiseSet?: InputMaybe<PredictedRiseSetCreateNestedManyWithoutStateElementInput>;
  epoch: Scalars["BigInt"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  stateElement: Scalars["Json"]["input"];
  tle?: InputMaybe<Scalars["String"]["input"]>;
}

export interface StateElementCreateManyInput {
  epoch: Scalars["BigInt"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  stateElement: Scalars["Json"]["input"];
  tle?: InputMaybe<Scalars["String"]["input"]>;
}

export interface StateElementCreateNestedOneWithoutRiseSetInput {
  connect?: InputMaybe<StateElementWhereUniqueInput>;
  connectOrCreate?: InputMaybe<StateElementCreateOrConnectWithoutRiseSetInput>;
  create?: InputMaybe<StateElementCreateWithoutRiseSetInput>;
}

export interface StateElementCreateOrConnectWithoutRiseSetInput {
  create: StateElementCreateWithoutRiseSetInput;
  where: StateElementWhereUniqueInput;
}

export interface StateElementCreateWithoutRiseSetInput {
  epoch: Scalars["BigInt"]["input"];
  id?: InputMaybe<Scalars["String"]["input"]>;
  stateElement: Scalars["Json"]["input"];
  tle?: InputMaybe<Scalars["String"]["input"]>;
}

export interface StateElementEpochStateElementCompoundUniqueInput {
  epoch: Scalars["BigInt"]["input"];
  stateElement: Scalars["Json"]["input"];
}

export interface StateElementMaxOrderByAggregateInput {
  epoch?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  tle?: InputMaybe<SortOrder>;
}

export interface StateElementMinOrderByAggregateInput {
  epoch?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  tle?: InputMaybe<SortOrder>;
}

export interface StateElementOrderByWithAggregationInput {
  _avg?: InputMaybe<StateElementAvgOrderByAggregateInput>;
  _count?: InputMaybe<StateElementCountOrderByAggregateInput>;
  _max?: InputMaybe<StateElementMaxOrderByAggregateInput>;
  _min?: InputMaybe<StateElementMinOrderByAggregateInput>;
  _sum?: InputMaybe<StateElementSumOrderByAggregateInput>;
  epoch?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  stateElement?: InputMaybe<SortOrder>;
  tle?: InputMaybe<SortOrder>;
}

export interface StateElementOrderByWithRelationInput {
  RiseSet?: InputMaybe<PredictedRiseSetOrderByRelationAggregateInput>;
  epoch?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  stateElement?: InputMaybe<SortOrder>;
  tle?: InputMaybe<SortOrder>;
}

export interface StateElementRelationFilter {
  is?: InputMaybe<StateElementWhereInput>;
  isNot?: InputMaybe<StateElementWhereInput>;
}

export enum StateElementScalarFieldEnum {
  Epoch = "epoch",
  Id = "id",
  StateElement = "stateElement",
  Tle = "tle",
}

export interface StateElementScalarWhereWithAggregatesInput {
  AND?: InputMaybe<StateElementScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<StateElementScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<StateElementScalarWhereWithAggregatesInput[]>;
  epoch?: InputMaybe<BigIntWithAggregatesFilter>;
  id?: InputMaybe<UuidWithAggregatesFilter>;
  stateElement?: InputMaybe<JsonWithAggregatesFilter>;
  tle?: InputMaybe<StringNullableWithAggregatesFilter>;
}

export interface StateElementSumOrderByAggregateInput {
  epoch?: InputMaybe<SortOrder>;
}

export interface StateElementUpdateInput {
  RiseSet?: InputMaybe<PredictedRiseSetUpdateManyWithoutStateElementNestedInput>;
  epoch?: InputMaybe<BigIntFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  stateElement?: InputMaybe<Scalars["Json"]["input"]>;
  tle?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
}

export interface StateElementUpdateManyMutationInput {
  epoch?: InputMaybe<BigIntFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  stateElement?: InputMaybe<Scalars["Json"]["input"]>;
  tle?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
}

export interface StateElementUpdateOneRequiredWithoutRiseSetNestedInput {
  connect?: InputMaybe<StateElementWhereUniqueInput>;
  connectOrCreate?: InputMaybe<StateElementCreateOrConnectWithoutRiseSetInput>;
  create?: InputMaybe<StateElementCreateWithoutRiseSetInput>;
  update?: InputMaybe<StateElementUpdateToOneWithWhereWithoutRiseSetInput>;
  upsert?: InputMaybe<StateElementUpsertWithoutRiseSetInput>;
}

export interface StateElementUpdateToOneWithWhereWithoutRiseSetInput {
  data: StateElementUpdateWithoutRiseSetInput;
  where?: InputMaybe<StateElementWhereInput>;
}

export interface StateElementUpdateWithoutRiseSetInput {
  epoch?: InputMaybe<BigIntFieldUpdateOperationsInput>;
  id?: InputMaybe<StringFieldUpdateOperationsInput>;
  stateElement?: InputMaybe<Scalars["Json"]["input"]>;
  tle?: InputMaybe<NullableStringFieldUpdateOperationsInput>;
}

export interface StateElementUpsertWithoutRiseSetInput {
  create: StateElementCreateWithoutRiseSetInput;
  update: StateElementUpdateWithoutRiseSetInput;
  where?: InputMaybe<StateElementWhereInput>;
}

export interface StateElementWhereInput {
  AND?: InputMaybe<StateElementWhereInput[]>;
  NOT?: InputMaybe<StateElementWhereInput[]>;
  OR?: InputMaybe<StateElementWhereInput[]>;
  RiseSet?: InputMaybe<PredictedRiseSetListRelationFilter>;
  epoch?: InputMaybe<BigIntFilter>;
  id?: InputMaybe<UuidFilter>;
  stateElement?: InputMaybe<JsonFilter>;
  tle?: InputMaybe<StringNullableFilter>;
}

export interface StateElementWhereUniqueInput {
  AND?: InputMaybe<StateElementWhereInput[]>;
  NOT?: InputMaybe<StateElementWhereInput[]>;
  OR?: InputMaybe<StateElementWhereInput[]>;
  RiseSet?: InputMaybe<PredictedRiseSetListRelationFilter>;
  epoch?: InputMaybe<BigIntFilter>;
  epoch_stateElement?: InputMaybe<StateElementEpochStateElementCompoundUniqueInput>;
  id?: InputMaybe<Scalars["String"]["input"]>;
  stateElement?: InputMaybe<JsonFilter>;
  tle?: InputMaybe<StringNullableFilter>;
}

export interface StringFieldUpdateOperationsInput {
  set?: InputMaybe<Scalars["String"]["input"]>;
}

export interface StringFilter {
  contains?: InputMaybe<Scalars["String"]["input"]>;
  endsWith?: InputMaybe<Scalars["String"]["input"]>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  mode?: InputMaybe<QueryMode>;
  not?: InputMaybe<NestedStringFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
  startsWith?: InputMaybe<Scalars["String"]["input"]>;
}

export interface StringNullableFilter {
  contains?: InputMaybe<Scalars["String"]["input"]>;
  endsWith?: InputMaybe<Scalars["String"]["input"]>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  mode?: InputMaybe<QueryMode>;
  not?: InputMaybe<NestedStringNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
  startsWith?: InputMaybe<Scalars["String"]["input"]>;
}

export interface StringNullableWithAggregatesFilter {
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedStringNullableFilter>;
  _min?: InputMaybe<NestedStringNullableFilter>;
  contains?: InputMaybe<Scalars["String"]["input"]>;
  endsWith?: InputMaybe<Scalars["String"]["input"]>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  mode?: InputMaybe<QueryMode>;
  not?: InputMaybe<NestedStringNullableWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
  startsWith?: InputMaybe<Scalars["String"]["input"]>;
}

export interface StringWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedStringFilter>;
  _min?: InputMaybe<NestedStringFilter>;
  contains?: InputMaybe<Scalars["String"]["input"]>;
  endsWith?: InputMaybe<Scalars["String"]["input"]>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  mode?: InputMaybe<QueryMode>;
  not?: InputMaybe<NestedStringWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
  startsWith?: InputMaybe<Scalars["String"]["input"]>;
}

export enum TransactionIsolationLevel {
  ReadCommitted = "ReadCommitted",
  ReadUncommitted = "ReadUncommitted",
  RepeatableRead = "RepeatableRead",
  Serializable = "Serializable",
}

export interface Transfer {
  __typename?: "Transfer";
  command: Command;
  commandId: Scalars["String"]["output"];
  id: Scalars["ID"]["output"];
  pass: Pass;
  passId: Scalars["String"]["output"];
  priority: Scalars["Int"]["output"];
  type: TransferType;
}

export interface TransferAvgOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  priority?: InputMaybe<SortOrder>;
}

export interface TransferCountOrderByAggregateInput {
  commandId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  priority?: InputMaybe<SortOrder>;
  type?: InputMaybe<SortOrder>;
}

export interface TransferCreateInput {
  command: CommandCreateNestedOneWithoutTransferInput;
  pass: PassCreateNestedOneWithoutTransferInput;
  priority: Scalars["Int"]["input"];
  type: TransferType;
}

export interface TransferCreateManyCommandInput {
  id?: InputMaybe<Scalars["Int"]["input"]>;
  passId: Scalars["String"]["input"];
  priority: Scalars["Int"]["input"];
  type: TransferType;
}

export interface TransferCreateManyCommandInputEnvelope {
  data: TransferCreateManyCommandInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface TransferCreateManyInput {
  commandId: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["Int"]["input"]>;
  passId: Scalars["String"]["input"];
  priority: Scalars["Int"]["input"];
  type: TransferType;
}

export interface TransferCreateManyPassInput {
  commandId: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["Int"]["input"]>;
  priority: Scalars["Int"]["input"];
  type: TransferType;
}

export interface TransferCreateManyPassInputEnvelope {
  data: TransferCreateManyPassInput[];
  skipDuplicates?: InputMaybe<Scalars["Boolean"]["input"]>;
}

export interface TransferCreateNestedManyWithoutCommandInput {
  connect?: InputMaybe<TransferWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<TransferCreateOrConnectWithoutCommandInput[]>;
  create?: InputMaybe<TransferCreateWithoutCommandInput[]>;
  createMany?: InputMaybe<TransferCreateManyCommandInputEnvelope>;
}

export interface TransferCreateNestedManyWithoutPassInput {
  connect?: InputMaybe<TransferWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<TransferCreateOrConnectWithoutPassInput[]>;
  create?: InputMaybe<TransferCreateWithoutPassInput[]>;
  createMany?: InputMaybe<TransferCreateManyPassInputEnvelope>;
}

export interface TransferCreateOrConnectWithoutCommandInput {
  create: TransferCreateWithoutCommandInput;
  where: TransferWhereUniqueInput;
}

export interface TransferCreateOrConnectWithoutPassInput {
  create: TransferCreateWithoutPassInput;
  where: TransferWhereUniqueInput;
}

export interface TransferCreateWithoutCommandInput {
  pass: PassCreateNestedOneWithoutTransferInput;
  priority: Scalars["Int"]["input"];
  type: TransferType;
}

export interface TransferCreateWithoutPassInput {
  command: CommandCreateNestedOneWithoutTransferInput;
  priority: Scalars["Int"]["input"];
  type: TransferType;
}

export interface TransferListRelationFilter {
  every?: InputMaybe<TransferWhereInput>;
  none?: InputMaybe<TransferWhereInput>;
  some?: InputMaybe<TransferWhereInput>;
}

export interface TransferMaxOrderByAggregateInput {
  commandId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  priority?: InputMaybe<SortOrder>;
  type?: InputMaybe<SortOrder>;
}

export interface TransferMinOrderByAggregateInput {
  commandId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  priority?: InputMaybe<SortOrder>;
  type?: InputMaybe<SortOrder>;
}

export interface TransferOrderByRelationAggregateInput {
  _count?: InputMaybe<SortOrder>;
}

export interface TransferOrderByWithAggregationInput {
  _avg?: InputMaybe<TransferAvgOrderByAggregateInput>;
  _count?: InputMaybe<TransferCountOrderByAggregateInput>;
  _max?: InputMaybe<TransferMaxOrderByAggregateInput>;
  _min?: InputMaybe<TransferMinOrderByAggregateInput>;
  _sum?: InputMaybe<TransferSumOrderByAggregateInput>;
  commandId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  passId?: InputMaybe<SortOrder>;
  priority?: InputMaybe<SortOrder>;
  type?: InputMaybe<SortOrder>;
}

export interface TransferOrderByWithRelationInput {
  command?: InputMaybe<CommandOrderByWithRelationInput>;
  commandId?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  pass?: InputMaybe<PassOrderByWithRelationInput>;
  passId?: InputMaybe<SortOrder>;
  priority?: InputMaybe<SortOrder>;
  type?: InputMaybe<SortOrder>;
}

export enum TransferScalarFieldEnum {
  CommandId = "commandId",
  Id = "id",
  PassId = "passId",
  Priority = "priority",
  Type = "type",
}

export interface TransferScalarWhereInput {
  AND?: InputMaybe<TransferScalarWhereInput[]>;
  NOT?: InputMaybe<TransferScalarWhereInput[]>;
  OR?: InputMaybe<TransferScalarWhereInput[]>;
  commandId?: InputMaybe<UuidFilter>;
  id?: InputMaybe<IntFilter>;
  passId?: InputMaybe<UuidFilter>;
  priority?: InputMaybe<IntFilter>;
  type?: InputMaybe<EnumTransferTypeFilter>;
}

export interface TransferScalarWhereWithAggregatesInput {
  AND?: InputMaybe<TransferScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<TransferScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<TransferScalarWhereWithAggregatesInput[]>;
  commandId?: InputMaybe<UuidWithAggregatesFilter>;
  id?: InputMaybe<IntWithAggregatesFilter>;
  passId?: InputMaybe<UuidWithAggregatesFilter>;
  priority?: InputMaybe<IntWithAggregatesFilter>;
  type?: InputMaybe<EnumTransferTypeWithAggregatesFilter>;
}

export interface TransferSumOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
  priority?: InputMaybe<SortOrder>;
}

export enum TransferType {
  Download = "download",
  Upload = "upload",
}

export interface TransferUpdateInput {
  command?: InputMaybe<CommandUpdateOneRequiredWithoutTransferNestedInput>;
  pass?: InputMaybe<PassUpdateOneRequiredWithoutTransferNestedInput>;
  priority?: InputMaybe<IntFieldUpdateOperationsInput>;
  type?: InputMaybe<EnumTransferTypeFieldUpdateOperationsInput>;
}

export interface TransferUpdateManyMutationInput {
  priority?: InputMaybe<IntFieldUpdateOperationsInput>;
  type?: InputMaybe<EnumTransferTypeFieldUpdateOperationsInput>;
}

export interface TransferUpdateManyWithWhereWithoutCommandInput {
  data: TransferUpdateManyMutationInput;
  where: TransferScalarWhereInput;
}

export interface TransferUpdateManyWithWhereWithoutPassInput {
  data: TransferUpdateManyMutationInput;
  where: TransferScalarWhereInput;
}

export interface TransferUpdateManyWithoutCommandNestedInput {
  connect?: InputMaybe<TransferWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<TransferCreateOrConnectWithoutCommandInput[]>;
  create?: InputMaybe<TransferCreateWithoutCommandInput[]>;
  createMany?: InputMaybe<TransferCreateManyCommandInputEnvelope>;
  delete?: InputMaybe<TransferWhereUniqueInput[]>;
  deleteMany?: InputMaybe<TransferScalarWhereInput[]>;
  disconnect?: InputMaybe<TransferWhereUniqueInput[]>;
  set?: InputMaybe<TransferWhereUniqueInput[]>;
  update?: InputMaybe<TransferUpdateWithWhereUniqueWithoutCommandInput[]>;
  updateMany?: InputMaybe<TransferUpdateManyWithWhereWithoutCommandInput[]>;
  upsert?: InputMaybe<TransferUpsertWithWhereUniqueWithoutCommandInput[]>;
}

export interface TransferUpdateManyWithoutPassNestedInput {
  connect?: InputMaybe<TransferWhereUniqueInput[]>;
  connectOrCreate?: InputMaybe<TransferCreateOrConnectWithoutPassInput[]>;
  create?: InputMaybe<TransferCreateWithoutPassInput[]>;
  createMany?: InputMaybe<TransferCreateManyPassInputEnvelope>;
  delete?: InputMaybe<TransferWhereUniqueInput[]>;
  deleteMany?: InputMaybe<TransferScalarWhereInput[]>;
  disconnect?: InputMaybe<TransferWhereUniqueInput[]>;
  set?: InputMaybe<TransferWhereUniqueInput[]>;
  update?: InputMaybe<TransferUpdateWithWhereUniqueWithoutPassInput[]>;
  updateMany?: InputMaybe<TransferUpdateManyWithWhereWithoutPassInput[]>;
  upsert?: InputMaybe<TransferUpsertWithWhereUniqueWithoutPassInput[]>;
}

export interface TransferUpdateWithWhereUniqueWithoutCommandInput {
  data: TransferUpdateWithoutCommandInput;
  where: TransferWhereUniqueInput;
}

export interface TransferUpdateWithWhereUniqueWithoutPassInput {
  data: TransferUpdateWithoutPassInput;
  where: TransferWhereUniqueInput;
}

export interface TransferUpdateWithoutCommandInput {
  pass?: InputMaybe<PassUpdateOneRequiredWithoutTransferNestedInput>;
  priority?: InputMaybe<IntFieldUpdateOperationsInput>;
  type?: InputMaybe<EnumTransferTypeFieldUpdateOperationsInput>;
}

export interface TransferUpdateWithoutPassInput {
  command?: InputMaybe<CommandUpdateOneRequiredWithoutTransferNestedInput>;
  priority?: InputMaybe<IntFieldUpdateOperationsInput>;
  type?: InputMaybe<EnumTransferTypeFieldUpdateOperationsInput>;
}

export interface TransferUpsertWithWhereUniqueWithoutCommandInput {
  create: TransferCreateWithoutCommandInput;
  update: TransferUpdateWithoutCommandInput;
  where: TransferWhereUniqueInput;
}

export interface TransferUpsertWithWhereUniqueWithoutPassInput {
  create: TransferCreateWithoutPassInput;
  update: TransferUpdateWithoutPassInput;
  where: TransferWhereUniqueInput;
}

export interface TransferWhereInput {
  AND?: InputMaybe<TransferWhereInput[]>;
  NOT?: InputMaybe<TransferWhereInput[]>;
  OR?: InputMaybe<TransferWhereInput[]>;
  command?: InputMaybe<CommandWhereInput>;
  commandId?: InputMaybe<UuidFilter>;
  id?: InputMaybe<IntFilter>;
  pass?: InputMaybe<PassWhereInput>;
  passId?: InputMaybe<UuidFilter>;
  priority?: InputMaybe<IntFilter>;
  type?: InputMaybe<EnumTransferTypeFilter>;
}

export interface TransferWhereUniqueInput {
  AND?: InputMaybe<TransferWhereInput[]>;
  NOT?: InputMaybe<TransferWhereInput[]>;
  OR?: InputMaybe<TransferWhereInput[]>;
  command?: InputMaybe<CommandWhereInput>;
  commandId?: InputMaybe<UuidFilter>;
  id?: InputMaybe<Scalars["Int"]["input"]>;
  pass?: InputMaybe<PassWhereInput>;
  passId?: InputMaybe<UuidFilter>;
  priority?: InputMaybe<IntFilter>;
  type?: InputMaybe<EnumTransferTypeFilter>;
}

export interface User {
  __typename?: "User";
  Log: Log[];
  admin?: Maybe<Admin>;
  email: Scalars["String"]["output"];
  id: Scalars["ID"]["output"];
  requestedCommand: Command[];
}

export interface UserLogArgs {
  cursor?: InputMaybe<LogWhereUniqueInput>;
  distinct?: InputMaybe<LogScalarFieldEnum[]>;
  orderBy?: InputMaybe<LogOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<LogWhereInput>;
}

export interface UserRequestedCommandArgs {
  cursor?: InputMaybe<CommandWhereUniqueInput>;
  distinct?: InputMaybe<CommandScalarFieldEnum[]>;
  orderBy?: InputMaybe<CommandOrderByWithRelationInput[]>;
  skip?: InputMaybe<Scalars["Int"]["input"]>;
  take?: InputMaybe<Scalars["Int"]["input"]>;
  where?: InputMaybe<CommandWhereInput>;
}

export interface UserAvgOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
}

export interface UserCountOrderByAggregateInput {
  email?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
}

export interface UserCreateInput {
  Log?: InputMaybe<LogCreateNestedManyWithoutUserInput>;
  admin?: InputMaybe<AdminCreateNestedOneWithoutUserInput>;
  email: Scalars["String"]["input"];
  requestedCommand?: InputMaybe<CommandCreateNestedManyWithoutUserInput>;
}

export interface UserCreateManyInput {
  email: Scalars["String"]["input"];
  id?: InputMaybe<Scalars["Int"]["input"]>;
}

export interface UserCreateNestedOneWithoutAdminInput {
  connect?: InputMaybe<UserWhereUniqueInput>;
  connectOrCreate?: InputMaybe<UserCreateOrConnectWithoutAdminInput>;
  create?: InputMaybe<UserCreateWithoutAdminInput>;
}

export interface UserCreateNestedOneWithoutLogInput {
  connect?: InputMaybe<UserWhereUniqueInput>;
  connectOrCreate?: InputMaybe<UserCreateOrConnectWithoutLogInput>;
  create?: InputMaybe<UserCreateWithoutLogInput>;
}

export interface UserCreateNestedOneWithoutRequestedCommandInput {
  connect?: InputMaybe<UserWhereUniqueInput>;
  connectOrCreate?: InputMaybe<UserCreateOrConnectWithoutRequestedCommandInput>;
  create?: InputMaybe<UserCreateWithoutRequestedCommandInput>;
}

export interface UserCreateOrConnectWithoutAdminInput {
  create: UserCreateWithoutAdminInput;
  where: UserWhereUniqueInput;
}

export interface UserCreateOrConnectWithoutLogInput {
  create: UserCreateWithoutLogInput;
  where: UserWhereUniqueInput;
}

export interface UserCreateOrConnectWithoutRequestedCommandInput {
  create: UserCreateWithoutRequestedCommandInput;
  where: UserWhereUniqueInput;
}

export interface UserCreateWithoutAdminInput {
  Log?: InputMaybe<LogCreateNestedManyWithoutUserInput>;
  email: Scalars["String"]["input"];
  requestedCommand?: InputMaybe<CommandCreateNestedManyWithoutUserInput>;
}

export interface UserCreateWithoutLogInput {
  admin?: InputMaybe<AdminCreateNestedOneWithoutUserInput>;
  email: Scalars["String"]["input"];
  requestedCommand?: InputMaybe<CommandCreateNestedManyWithoutUserInput>;
}

export interface UserCreateWithoutRequestedCommandInput {
  Log?: InputMaybe<LogCreateNestedManyWithoutUserInput>;
  admin?: InputMaybe<AdminCreateNestedOneWithoutUserInput>;
  email: Scalars["String"]["input"];
}

export interface UserMaxOrderByAggregateInput {
  email?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
}

export interface UserMinOrderByAggregateInput {
  email?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
}

export interface UserOrderByWithAggregationInput {
  _avg?: InputMaybe<UserAvgOrderByAggregateInput>;
  _count?: InputMaybe<UserCountOrderByAggregateInput>;
  _max?: InputMaybe<UserMaxOrderByAggregateInput>;
  _min?: InputMaybe<UserMinOrderByAggregateInput>;
  _sum?: InputMaybe<UserSumOrderByAggregateInput>;
  email?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
}

export interface UserOrderByWithRelationInput {
  Log?: InputMaybe<LogOrderByRelationAggregateInput>;
  admin?: InputMaybe<AdminOrderByWithRelationInput>;
  email?: InputMaybe<SortOrder>;
  id?: InputMaybe<SortOrder>;
  requestedCommand?: InputMaybe<CommandOrderByRelationAggregateInput>;
}

export interface UserRelationFilter {
  is?: InputMaybe<UserWhereInput>;
  isNot?: InputMaybe<UserWhereInput>;
}

export enum UserScalarFieldEnum {
  Email = "email",
  Id = "id",
}

export interface UserScalarWhereWithAggregatesInput {
  AND?: InputMaybe<UserScalarWhereWithAggregatesInput[]>;
  NOT?: InputMaybe<UserScalarWhereWithAggregatesInput[]>;
  OR?: InputMaybe<UserScalarWhereWithAggregatesInput[]>;
  email?: InputMaybe<StringWithAggregatesFilter>;
  id?: InputMaybe<IntWithAggregatesFilter>;
}

export interface UserSumOrderByAggregateInput {
  id?: InputMaybe<SortOrder>;
}

export interface UserUpdateInput {
  Log?: InputMaybe<LogUpdateManyWithoutUserNestedInput>;
  admin?: InputMaybe<AdminUpdateOneWithoutUserNestedInput>;
  email?: InputMaybe<StringFieldUpdateOperationsInput>;
  requestedCommand?: InputMaybe<CommandUpdateManyWithoutUserNestedInput>;
}

export interface UserUpdateManyMutationInput {
  email?: InputMaybe<StringFieldUpdateOperationsInput>;
}

export interface UserUpdateOneRequiredWithoutAdminNestedInput {
  connect?: InputMaybe<UserWhereUniqueInput>;
  connectOrCreate?: InputMaybe<UserCreateOrConnectWithoutAdminInput>;
  create?: InputMaybe<UserCreateWithoutAdminInput>;
  update?: InputMaybe<UserUpdateToOneWithWhereWithoutAdminInput>;
  upsert?: InputMaybe<UserUpsertWithoutAdminInput>;
}

export interface UserUpdateOneRequiredWithoutLogNestedInput {
  connect?: InputMaybe<UserWhereUniqueInput>;
  connectOrCreate?: InputMaybe<UserCreateOrConnectWithoutLogInput>;
  create?: InputMaybe<UserCreateWithoutLogInput>;
  update?: InputMaybe<UserUpdateToOneWithWhereWithoutLogInput>;
  upsert?: InputMaybe<UserUpsertWithoutLogInput>;
}

export interface UserUpdateOneRequiredWithoutRequestedCommandNestedInput {
  connect?: InputMaybe<UserWhereUniqueInput>;
  connectOrCreate?: InputMaybe<UserCreateOrConnectWithoutRequestedCommandInput>;
  create?: InputMaybe<UserCreateWithoutRequestedCommandInput>;
  update?: InputMaybe<UserUpdateToOneWithWhereWithoutRequestedCommandInput>;
  upsert?: InputMaybe<UserUpsertWithoutRequestedCommandInput>;
}

export interface UserUpdateToOneWithWhereWithoutAdminInput {
  data: UserUpdateWithoutAdminInput;
  where?: InputMaybe<UserWhereInput>;
}

export interface UserUpdateToOneWithWhereWithoutLogInput {
  data: UserUpdateWithoutLogInput;
  where?: InputMaybe<UserWhereInput>;
}

export interface UserUpdateToOneWithWhereWithoutRequestedCommandInput {
  data: UserUpdateWithoutRequestedCommandInput;
  where?: InputMaybe<UserWhereInput>;
}

export interface UserUpdateWithoutAdminInput {
  Log?: InputMaybe<LogUpdateManyWithoutUserNestedInput>;
  email?: InputMaybe<StringFieldUpdateOperationsInput>;
  requestedCommand?: InputMaybe<CommandUpdateManyWithoutUserNestedInput>;
}

export interface UserUpdateWithoutLogInput {
  admin?: InputMaybe<AdminUpdateOneWithoutUserNestedInput>;
  email?: InputMaybe<StringFieldUpdateOperationsInput>;
  requestedCommand?: InputMaybe<CommandUpdateManyWithoutUserNestedInput>;
}

export interface UserUpdateWithoutRequestedCommandInput {
  Log?: InputMaybe<LogUpdateManyWithoutUserNestedInput>;
  admin?: InputMaybe<AdminUpdateOneWithoutUserNestedInput>;
  email?: InputMaybe<StringFieldUpdateOperationsInput>;
}

export interface UserUpsertWithoutAdminInput {
  create: UserCreateWithoutAdminInput;
  update: UserUpdateWithoutAdminInput;
  where?: InputMaybe<UserWhereInput>;
}

export interface UserUpsertWithoutLogInput {
  create: UserCreateWithoutLogInput;
  update: UserUpdateWithoutLogInput;
  where?: InputMaybe<UserWhereInput>;
}

export interface UserUpsertWithoutRequestedCommandInput {
  create: UserCreateWithoutRequestedCommandInput;
  update: UserUpdateWithoutRequestedCommandInput;
  where?: InputMaybe<UserWhereInput>;
}

export interface UserWhereInput {
  AND?: InputMaybe<UserWhereInput[]>;
  Log?: InputMaybe<LogListRelationFilter>;
  NOT?: InputMaybe<UserWhereInput[]>;
  OR?: InputMaybe<UserWhereInput[]>;
  admin?: InputMaybe<AdminWhereInput>;
  email?: InputMaybe<StringFilter>;
  id?: InputMaybe<IntFilter>;
  requestedCommand?: InputMaybe<CommandListRelationFilter>;
}

export interface UserWhereUniqueInput {
  AND?: InputMaybe<UserWhereInput[]>;
  Log?: InputMaybe<LogListRelationFilter>;
  NOT?: InputMaybe<UserWhereInput[]>;
  OR?: InputMaybe<UserWhereInput[]>;
  admin?: InputMaybe<AdminWhereInput>;
  email?: InputMaybe<Scalars["String"]["input"]>;
  id?: InputMaybe<Scalars["Int"]["input"]>;
  requestedCommand?: InputMaybe<CommandListRelationFilter>;
}

export interface UuidFilter {
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  mode?: InputMaybe<QueryMode>;
  not?: InputMaybe<NestedUuidFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
}

export interface UuidNullableFilter {
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  mode?: InputMaybe<QueryMode>;
  not?: InputMaybe<NestedUuidNullableFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
}

export interface UuidNullableWithAggregatesFilter {
  _count?: InputMaybe<NestedIntNullableFilter>;
  _max?: InputMaybe<NestedStringNullableFilter>;
  _min?: InputMaybe<NestedStringNullableFilter>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  mode?: InputMaybe<QueryMode>;
  not?: InputMaybe<NestedUuidNullableWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
}

export interface UuidWithAggregatesFilter {
  _count?: InputMaybe<NestedIntFilter>;
  _max?: InputMaybe<NestedStringFilter>;
  _min?: InputMaybe<NestedStringFilter>;
  equals?: InputMaybe<Scalars["String"]["input"]>;
  gt?: InputMaybe<Scalars["String"]["input"]>;
  gte?: InputMaybe<Scalars["String"]["input"]>;
  in?: InputMaybe<Array<Scalars["String"]["input"]>>;
  lt?: InputMaybe<Scalars["String"]["input"]>;
  lte?: InputMaybe<Scalars["String"]["input"]>;
  mode?: InputMaybe<QueryMode>;
  not?: InputMaybe<NestedUuidWithAggregatesFilter>;
  notIn?: InputMaybe<Array<Scalars["String"]["input"]>>;
}

export type WebhooksNewOrbitPropagationInitQueryQueryVariables = Exact<{
  timeAfter?: InputMaybe<Scalars["Int"]["input"]>;
}>;

export interface WebhooksNewOrbitPropagationInitQueryQuery {
  __typename?: "Query";
  findManyGroundStation: Array<{
    __typename?: "GroundStation";
    id: string;
    name: string;
    lat: number;
    lng: number;
    minElevation: number;
  }>;
  findManyPass: Array<{
    __typename?: "Pass";
    id: string;
    groundStationId: number;
    predictedRiseSet: {
      __typename?: "PredictedRiseSet";
      id: string;
      rise: number;
      set: number;
    };
  }>;
}

export type PassSchedulingUpdatePassMutationVariables = Exact<{
  deletePasses?: InputMaybe<
    PassSchedulingUpdatePassDeleteInput[] | PassSchedulingUpdatePassDeleteInput
  >;
  appendPasses?: InputMaybe<
    PassSchedulingUpdatePassAppendInput[] | PassSchedulingUpdatePassAppendInput
  >;
  insertPasses?: InputMaybe<
    PassSchedulingUpdatePassInsertInput[] | PassSchedulingUpdatePassInsertInput
  >;
  updatePasses?: InputMaybe<
    PassSchedulingUpdatePassUpdateInput[] | PassSchedulingUpdatePassUpdateInput
  >;
}>;

export interface PassSchedulingUpdatePassMutation {
  __typename?: "Mutation";
  passSchedulingUpdatePass: boolean;
}

export const WebhooksNewOrbitPropagationInitQueryDocument = gql`
  query webhooksNewOrbitPropagationInitQuery($timeAfter: Int) {
    findManyGroundStation {
      id
      name
      lat
      lng
      minElevation
    }
    findManyPass(
      where: { predictedRiseSet: { rise: { gte: $timeAfter } } }
      orderBy: { predictedRiseSet: { rise: asc } }
    ) {
      id
      predictedRiseSet {
        id
        rise
        set
      }
      groundStationId
    }
  }
`;
export const PassSchedulingUpdatePassDocument = gql`
  mutation passSchedulingUpdatePass(
    $deletePasses: [PassSchedulingUpdatePassDeleteInput!]
    $appendPasses: [PassSchedulingUpdatePassAppendInput!]
    $insertPasses: [PassSchedulingUpdatePassInsertInput!]
    $updatePasses: [PassSchedulingUpdatePassUpdateInput!]
  ) {
    passSchedulingUpdatePass(
      append: $appendPasses
      insert: $insertPasses
      delete: $deletePasses
      update: $updatePasses
    )
  }
`;

export type SdkFunctionWrapper = <T>(
  action: (requestHeaders?: Record<string, string>) => Promise<T>,
  operationName: string,
  operationType?: string,
  variables?: any,
) => Promise<T>;

const defaultWrapper: SdkFunctionWrapper = async (
  action,
  _operationName,
  _operationType,
  variables,
) => await action();

export function getSdk(
  client: GraphQLClient,
  withWrapper: SdkFunctionWrapper = defaultWrapper,
) {
  return {
    async webhooksNewOrbitPropagationInitQuery(
      variables?: WebhooksNewOrbitPropagationInitQueryQueryVariables,
      requestHeaders?: GraphQLClientRequestHeaders,
    ): Promise<WebhooksNewOrbitPropagationInitQueryQuery> {
      return await withWrapper(
        async (wrappedRequestHeaders) =>
          await client.request<WebhooksNewOrbitPropagationInitQueryQuery>(
            WebhooksNewOrbitPropagationInitQueryDocument,
            variables,
            { ...requestHeaders, ...wrappedRequestHeaders },
          ),
        "webhooksNewOrbitPropagationInitQuery",
        "query",
        variables,
      );
    },
    async passSchedulingUpdatePass(
      variables?: PassSchedulingUpdatePassMutationVariables,
      requestHeaders?: GraphQLClientRequestHeaders,
    ): Promise<PassSchedulingUpdatePassMutation> {
      return await withWrapper(
        async (wrappedRequestHeaders) =>
          await client.request<PassSchedulingUpdatePassMutation>(
            PassSchedulingUpdatePassDocument,
            variables,
            { ...requestHeaders, ...wrappedRequestHeaders },
          ),
        "passSchedulingUpdatePass",
        "mutation",
        variables,
      );
    },
  };
}
export type Sdk = ReturnType<typeof getSdk>;
