import SearchIcon from "@mui/icons-material/Search";
import IconButton from "@mui/material/IconButton";
import InputBase from "@mui/material/InputBase";
import Paper from "@mui/material/Paper";

export default function SearchBar() {
  return (
    <Paper
      component="form"
      sx={{
        pl: 2,
        display: "flex",
        alignItems: "center",
        width: 600,
        borderRadius: "30px",
        border: "2px solid white",
      }}
    >
      <InputBase
        sx={{ ml: 1, flex: 1 }}
        placeholder="Search by username, image description, timestamp"
        inputProps={{ "aria-label": "search google maps" }}
        fullWidth
      />
      <IconButton type="submit" sx={{ p: 1 }} aria-label="search">
        <SearchIcon />
      </IconButton>
    </Paper>
  );
}
