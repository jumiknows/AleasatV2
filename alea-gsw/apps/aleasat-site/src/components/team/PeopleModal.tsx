import type { MemberData } from "@/data/members";
import Link from "@aleasat/ui/components/Link";
import CloseIcon from "@mui/icons-material/Close";
import GitHubIcon from "@mui/icons-material/GitHub";
import LinkedInIcon from "@mui/icons-material/LinkedIn";
import { Box, Chip, IconButton, Modal, Stack, Typography } from "@mui/material";
import CardMedia from "@mui/material/CardMedia";
import * as React from "react";

export default function PeopleModal({
  open,
  handleClose,
  person,
}: {
  open: boolean;
  handleClose: () => void;
  person: MemberData;
}) {
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
    <>
      <Modal
        open={open}
        onClose={handleClose}
        data-cy="member-modal"
        data-member={personName}
        sx={{
          display: { xs: "block", md: "none" },
        }}
      >
        <Box
          sx={{
            position: "absolute",
            top: "50%",
            left: "50%",
            transform: "translate(-50%, -50%)",
            bgcolor: "background.paper",
            p: 3,
            width: "90%",
            maxWidth: 550,
            borderRadius: 4,
            border: "white 1px solid",
            gap: { xs: 1.5, sm: 2 },
            display: "flex",
            flexDirection: "column",
          }}
        >
          <IconButton
            aria-label="close"
            data-cy="modal-close-button"
            onClick={handleClose}
            sx={{
              position: "absolute",
              top: 8,
              right: 8,
              color: "grey.500",
            }}
          >
            <CloseIcon />
          </IconButton>
          <Box
            sx={{
              display: "flex",
              flexDirection: "column",
              gap: { xs: 1, sm: 2 },
            }}
          >
            <CardMedia
              component="img"
              data-cy="modal-member-image"
              data-member={personName}
              image={
                person.image
                  ? `https://artifact.aleasat.space/team-photo/${person.image}`
                  : `https://artifact.aleasat.space/team-photo/Jankey.png`
              }
              alt={`${personName}'s photo`}
              sx={{
                minWidth: 140,
                width: "100%",
                maxWidth: 550,
                aspectRatio: "1/1",
                borderRadius: 5,
                border: !person.image ? "2px solid" : undefined,
              }}
            />
          </Box>
          <Box
            sx={{
              display: "flex",
              gap: { xs: 1 },
              justifyContent: "start",
              flexWrap: "wrap",
            }}
          >
            {personTags.map((tag, i) => (
              <Chip
                key={i}
                variant="outlined"
                data-cy="modal-tag"
                data-tag={tag}
                sx={{
                  border: "white 1px solid",
                  maxWidth: "max-content",
                }}
                label={tag}
              />
            ))}
          </Box>
          <Box
            sx={{
              order: { xs: -1, sm: -1 },
            }}
          >
            <Typography
              variant="h4"
              data-cy="modal-member-name"
              data-member={personName}
            >
              {personName}
            </Typography>
            <Typography
              variant="subtitle1"
              color="text.secondary"
              data-cy="modal-member-role"
              data-member={personName}
            >
              {person.role}
            </Typography>
          </Box>
          <Stack
            direction="row"
            spacing={2}
            sx={{
              marginTop: { xs: 0 },
              alignItems: "center",
            }}
          >
            {person.socialMedia.github && (
              <Link
                href={person.socialMedia.github}
                data-cy="modal-github-link"
                data-member={personName}
              >
                <GitHubIcon fontSize="large" color="secondary" />
              </Link>
            )}
            {person.socialMedia.linkedin && (
              <Link
                href={person.socialMedia.linkedin}
                data-cy="modal-linkedin-link"
                data-member={personName}
              >
                <LinkedInIcon fontSize="large" color="secondary" />
              </Link>
            )}
          </Stack>
        </Box>
      </Modal>
    </>
  );
}
