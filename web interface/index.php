<?php
/* The relative path that needs to be taken to the webroot from the current location of this file */
$path_to_root = './';

////////////////////////////////////////
/* Main entry point for Ping Monitor  */
////////////////////////////////////////

?>
<!DOCTYPE html>
<html lang="en">
	<head>
<?php
	include ( "{$path_to_root}php/header.php" );
?>
		<script>
			refresh_grid( );
			setInterval( refresh_grid, 10000 );
		</script>
	</head>
	<body>
		<!--Main content of page-->
		<div class="container">
			<div id="grid">
				<div id="loading">
					<p>Loading...</p>
				</div>
			</div>
		</div>
	</body>
</html>	