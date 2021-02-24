<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)): ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<?php foreach ($rows as $id => $row): ?>

  <?php if (isset($prefix_row_key) && $id == $prefix_row_key): ?>
    <button class="btn btn-default btn-xs btn-archive" type="button" data-toggle="collapse" data-target="#collapse-<? print $id; ?>" aria-expanded="false" aria-controls="collapse-<? print $id; ?>">
      Архив<?php print '';?>
    </button>
    <div class="collapse" id="collapse-<? print $id; ?>">
  <?php endif; ?>

  <div<?php if ($classes_array[$id]): ?> class="<?php print $classes_array[$id]; ?>"<?php endif; ?>>
    <?php print $row; ?>
  </div>

  <?php if (isset($suffix_row_key) && $id == $suffix_row_key): ?>
    </div><?php print '';?>
  <?php endif; ?>

<?php endforeach; ?>
