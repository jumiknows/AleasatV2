import type { MemberData } from "@/data/members";
import Link from "@aleasat/ui/components/Link";
import GitHubIcon from "@mui/icons-material/GitHub";
import LinkedInIcon from "@mui/icons-material/LinkedIn";
import { Box, Chip, Stack, Typography } from "@mui/material";
import Card from "@mui/material/Card";
import CardHeader from "@mui/material/CardHeader";
import CardMedia from "@mui/material/CardMedia";
import Image from "next/image";
import { useState } from "react";
import PeopleModal from "./PeopleModal";

export default function PeopleCard({ person }: { person: MemberData }) {
  const [hover, setHover] = useState(false);
  const [open, setOpen] = useState(false);

  const personName = `${person.firstname} ${person.lastname}`;
  const personTags = [
    person.university,
    person.year === "M"
      ? "Master"
      : person.year === "G"
        ? "Graduated"
        : `Year ${person.year}`,
    person.major,
  ];

  return (
    <Stack
      data-cy="team-member-card"
      data-member={personName}
      onClick={() => {
        if (window.innerWidth < 600) {
          setOpen(!open);
        }
      }}
      sx={{
        bgcolor: "transparent",
        backgroundImage: "unset",
      }}
      direction="row"
      justifyContent="center"
      alignItems="center"
    >
      <Box
        sx={{
          position: "relative",
          width: "50px",
          aspectRatio: "1/1",
          overflow: "hidden",
          border: !person.image ? "2px solid" : undefined,
          borderRadius: "5%",
        }}
      >
        <Box
          sx={{
            position: "absolute",
            width: person.image?.startsWith("members/") ? "200%" : "100%",
            aspectRatio: person.image ? "2/3" : "unset",
            height: person.image ? "unset" : 1,
            transform: "translate(-50%, -50%)",
            left: "50%",
            top: person.image?.startsWith("members/") ? "90%" : "50%",
          }}
        >
          <Image
            data-cy="team-member-image"
            data-member={personName}
            src={
              person.image
                ? `https://artifact.aleasat.space/team-photo/${person.image}`
                : `https://artifact.aleasat.space/team-photo/Jankey.png`
            }
            alt={`${personName}'s photo`}
            sizes="50px"
            fill
            style={{
              objectFit: "cover",
            }}
          />
        </Box>
        <Box
          onMouseEnter={() => setHover(true)}
          onMouseLeave={() => setHover(false)}
          sx={{
            position: "absolute",
            top: 0,
            left: 0,
            right: 0,
            bottom: 0,
          }}
        >
          {hover && (
            <Box
              data-cy="team-member-hover-overlay"
              data-member={personName}
              sx={{
                width: "100%",
                height: "100%",
                bgcolor: "rgba(0,0,0,0.7)",
                padding: "10px",
              }}
            >
              <Stack
                sx={{
                  justifyContent: "space-between",
                  height: "100%",
                }}
              >
                <Stack
                  direction="row"
                  spacing={1}
                  useFlexGap
                  sx={{ flexWrap: "wrap" }}
                >
                  {personTags.map((tag, i) => (
                    <Chip
                      key={i}
                      variant="outlined"
                      label={tag}
                      data-cy="member-tag"
                      data-tag={tag}
                    />
                  ))}
                </Stack>
                <Stack direction="row" spacing={2}>
                  {person.socialMedia.github && (
                    <Link
                      href={person.socialMedia.github}
                      data-cy="github-link"
                      data-member={personName}
                    >
                      <GitHubIcon fontSize="large" color="secondary" />
                    </Link>
                  )}
                  {person.socialMedia.linkedin && (
                    <Link
                      href={person.socialMedia.linkedin}
                      data-cy="linkedin-link"
                      data-member={personName}
                    >
                      <LinkedInIcon fontSize="large" color="secondary" />
                    </Link>
                  )}
                </Stack>
              </Stack>
            </Box>
          )}
        </Box>
      </Box>
      <CardHeader
        sx={{
          textAlign: "left",
          textWrap: "wrap",
          py: 0,
          px: 1,
          width: "130px",
        }}
        title={
          <Typography
            data-cy="team-member-name"
            data-member={personName}
            sx={{
              fontSize: "0.8rem",
              textWrap: "wrap",
            }}
          >
            {personName}
          </Typography>
        }
        subheader={
          <Typography
            variant="body2"
            data-cy="team-member-role"
            data-member={personName}
            sx={{
              fontSize: "0.7rem",
              textWrap: "nowrap",
            }}
          >
            {person.role}
          </Typography>
        }
      />
      {/* <PeopleModal
        person={person}
        open={open}
        handleClose={() => {
          setOpen(false);
        }}
      /> */}
    </Stack>
  );
}
