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

 Date: 29/04/2021 02:22:00
*/


-- ----------------------------
-- Table structure for nodes_cmd
-- ----------------------------
DROP TABLE IF EXISTS "public"."nodes_cmd";
CREATE TABLE "public"."nodes_cmd" (
  "id" varchar(50) COLLATE "pg_catalog"."default" NOT NULL DEFAULT uuid_generate_v4(),
  "node" varchar(3) COLLATE "pg_catalog"."default",
  "req" varchar(3) COLLATE "pg_catalog"."default",
  "data" varchar(3) COLLATE "pg_catalog"."default",
  "ctime" timestamp(6) DEFAULT now()
)
;

-- ----------------------------
-- Primary Key structure for table nodes_cmd
-- ----------------------------
ALTER TABLE "public"."nodes_cmd" ADD CONSTRAINT "nodes_copy1_pkey" PRIMARY KEY ("id");
