/*
 Navicat Premium Data Transfer

 Source Server         : LOCAL_PG
 Source Server Type    : PostgreSQL
 Source Server Version : 100013
 Source Host           : 127.0.0.1:5432
 Source Catalog        : postgres
 Source Schema         : public

 Target Server Type    : PostgreSQL
 Target Server Version : 100013
 File Encoding         : 65001

 Date: 13/04/2021 19:00:38
*/


-- ----------------------------
-- Table structure for nodes
-- ----------------------------
DROP TABLE IF EXISTS "public"."nodes";
CREATE TABLE "public"."nodes" (
  "id" varchar(50) COLLATE "pg_catalog"."default" NOT NULL DEFAULT uuid_generate_v4(),
  "serial_number" varchar(10) COLLATE "pg_catalog"."default",
  "node" varchar(10) COLLATE "pg_catalog"."default",
  "battery" float4,
  "data" varchar(255) COLLATE "pg_catalog"."default"
)
;

-- ----------------------------
-- Primary Key structure for table nodes
-- ----------------------------
ALTER TABLE "public"."nodes" ADD CONSTRAINT "nodes_pkey" PRIMARY KEY ("id");
